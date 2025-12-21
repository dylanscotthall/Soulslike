#include "vkFrame.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

vFrame::vFrame(vDevice &device, VkSwapchainKHR swapchain, int maxFramesInFlight)
    : device(device), MAX_FRAMES_IN_FLIGHT(maxFramesInFlight) {

  uint32_t imageCount = 0;
  vkGetSwapchainImagesKHR(device.getLogical(), swapchain, &imageCount, nullptr);
  imagesInFlight.resize(imageCount, VK_NULL_HANDLE);

  imageAvailableSemaphores.resize(imageCount);
  renderFinishedSemaphores.resize(imageCount);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateFence(device.getLogical(), &fenceInfo, nullptr,
                      &inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create fence!");
    }
  }

  for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
    if (vkCreateSemaphore(device.getLogical(), &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device.getLogical(), &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create semaphores!");
    }
  }
}

void vFrame::drawFrame(uint32_t &currentFrame, vSwapchain &swapchain,
                       VkSurfaceKHR surface, Window &window,
                       VkBuffer vertexBuffer, vCommand &command,
                       vPipeline &pipeline) {

  // Wait for the current frame to finish
  vkWaitForFences(device.getLogical(), 1, &inFlightFences[currentFrame],
                  VK_TRUE, UINT64_MAX);

  // Acquire next image
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      device.getLogical(), swapchain.getSwapchain(), UINT64_MAX,
      imageAvailableSemaphores[currentFrame], // use semaphore for this frame
      VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    swapchain.recreateSwapchain(surface, window);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swapchain image!");
  }

  // Wait if this image is already in flight
  if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(device.getLogical(), 1, &imagesInFlight[imageIndex],
                    VK_TRUE, UINT64_MAX);
  }

  // Mark image as now being in use by this frame
  imagesInFlight[imageIndex] = inFlightFences[currentFrame];

  vkResetFences(device.getLogical(), 1, &inFlightFences[currentFrame]);

  // Record commands for this frame/image
  vkResetCommandBuffer(command.getCommandBuffers()[currentFrame], 0);
  command.recordCommandBuffer(command.getCommandBuffers()[currentFrame],
                              imageIndex, swapchain, pipeline, vertexBuffer);

  // Submit command buffer
  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command.getCommandBuffers()[currentFrame];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo,
                    inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("Failed to submit draw command buffer!");
  }

  // Present
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapchains[] = {swapchain.getSwapchain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.getFrameBufferResized()) {
    window.setFrameBufferResized(false);
    swapchain.recreateSwapchain(surface, window);
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swapchain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
vFrame::~vFrame() {
  for (auto semaphore : imageAvailableSemaphores) {
    vkDestroySemaphore(device.getLogical(), semaphore, nullptr);
  }
  for (auto semaphore : renderFinishedSemaphores) {
    vkDestroySemaphore(device.getLogical(), semaphore, nullptr);
  }
  for (auto fence : inFlightFences) {
    vkDestroyFence(device.getLogical(), fence, nullptr);
  }
}
