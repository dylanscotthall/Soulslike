#include "vkFrame.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

vFrame::vFrame(vDevice &device) : device(device) {
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device.getLogical(), &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device.getLogical(), &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device.getLogical(), &fenceInfo, nullptr,
                      &inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create syncronisation objects for a frame!");
    }
  }
}

void vFrame::drawFrame(uint32_t currentFrame, vSwapchain &swapchain,
                       VkSurfaceKHR surface, VkRenderPass renderPass,
                       Window &window, VkBuffer vertexBuffer, vCommand &command,
                       vPipeline &pipeline) {
  vkWaitForFences(device.getLogical(), 1, &inFlightFences[currentFrame],
                  VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      device.getLogical(), swapchain.getSwapchain(), UINT64_MAX,
      imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    swapchain.recreateSwapchain(surface, window, renderPass);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  vkResetFences(device.getLogical(), 1, &inFlightFences[currentFrame]);

  vkResetCommandBuffer(command.getCommandBuffers()[currentFrame], 0);
  command.recordCommandBuffer(command.getCommandBuffers()[currentFrame],
                              imageIndex, renderPass, swapchain, pipeline,
                              vertexBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command.getCommandBuffers()[currentFrame];

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo,
                    inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapchains[] = {swapchain.getSwapchain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.getFrameBufferResized()) {
    window.setFrameBufferResized(false);
    swapchain.recreateSwapchain(surface, window, renderPass);
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

vFrame::~vFrame() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device.getLogical(), imageAvailableSemaphores[i],
                       nullptr);
    vkDestroySemaphore(device.getLogical(), renderFinishedSemaphores[i],
                       nullptr);
    vkDestroyFence(device.getLogical(), inFlightFences[i], nullptr);
  }
}

std::vector<VkSemaphore> vFrame::getImageAvailableSemaphores() const noexcept {
  return imageAvailableSemaphores;
}
std::vector<VkSemaphore> vFrame::getRenderFinishedSemaphores() const noexcept {
  return renderFinishedSemaphores;
}
std::vector<VkFence> vFrame::getInFlightFences() const noexcept {
  return inFlightFences;
}
const int vFrame::getMaxFramesInFlight() const noexcept {
  return MAX_FRAMES_IN_FLIGHT;
}
