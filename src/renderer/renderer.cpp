#include "renderer.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Renderer::Renderer(Device &device, Swapchain &swapchain, Command &command,
                   Frame &frame)
    : device(device), swapchain(swapchain), command(command), frame(frame) {}

RenderResult Renderer::drawFrame(std::span<const RenderItem> items) {
  // Wait for the current frame to finish
  vkWaitForFences(device.getLogical(), 1, &frame.getInFlightFence(currentFrame),
                  VK_TRUE, UINT64_MAX);

  // Acquire next image
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      device.getLogical(), swapchain.getSwapchain(), UINT64_MAX,
      frame.getImageAvailableSemaphore(currentFrame), // use semaphore for
                                                      // this frame
      VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return RenderResult::SwapchainOutOfDate;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swapchain image!");
  }

  // Wait if this image is already in flight
  if (frame.getImageInFlight(imageIndex) != VK_NULL_HANDLE) {
    vkWaitForFences(device.getLogical(), 1, &frame.getImageInFlight(imageIndex),
                    VK_TRUE, UINT64_MAX);
  }

  // Mark image as now being in use by this frame
  frame.setImagesInFlight(imageIndex, frame.getInFlightFence(currentFrame));

  vkResetFences(device.getLogical(), 1, &frame.getInFlightFence(currentFrame));

  // Record commands for this frame/image
  vkResetCommandBuffer(command.getCommandBuffers()[currentFrame], 0);
  command.recordCommandBuffer(command.getCommandBuffers()[currentFrame],
                              imageIndex, swapchain, items);

  // Submit command buffer
  VkSemaphore waitSemaphores[] = {
      frame.getImageAvailableSemaphore(currentFrame)};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[] = {
      frame.getRenderFinishedSemaphore(imageIndex)};

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
                    frame.getInFlightFence(currentFrame)) != VK_SUCCESS) {
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

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    return RenderResult::SwapchainOutOfDate;
  }
  if (result != VK_SUCCESS) {
    return RenderResult::FatalError;
  }

  currentFrame = (currentFrame + 1) % frame.getMaxFramesInFlight();
  return RenderResult::Ok;
}

Renderer::~Renderer() {}
