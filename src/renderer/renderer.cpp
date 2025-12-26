#include "renderer/renderer.h"
#include "helper.h"
#include "rhi/vulkan/commandContext.h"
#include "rhi/vulkan/device.h"
#include "rhi/vulkan/renderRecorder.h"
#include "rhi/vulkan/swapchain.h"

Renderer::Renderer(Device &device, Swapchain &swapchain,
                   CommandContext &commands, RenderRecorder &recorder,
                   Frame &frame)
    : device(device), swapchain(swapchain), commands(commands),
      recorder(recorder), frame(frame) {}

RenderResult Renderer::drawFrame(std::span<RenderItem *> items,
                                 Camera &camera) {
  auto &fence = frame.getInFlightFence(currentFrame);
  vkWaitForFences(device.getLogical(), 1, &fence, VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult res = vkAcquireNextImageKHR(
      device.getLogical(), swapchain.getSwapchain(), UINT64_MAX,
      frame.getImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE,
      &imageIndex);

  if (res == VK_ERROR_OUT_OF_DATE_KHR)
    return RenderResult::SwapchainOutOfDate;

  if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
    return RenderResult::FatalError;

  // Wait if this image is already in flight
  VkFence &imageFence = frame.getImageInFlight(imageIndex);
  if (imageFence != VK_NULL_HANDLE) {
    vkWaitForFences(device.getLogical(), 1, &imageFence, VK_TRUE, UINT64_MAX);
  }

  // Mark image as now using this frame's fence
  frame.setImageInFlight(imageIndex, fence);

  vkResetFences(device.getLogical(), 1, &fence);

  VkCommandBuffer cmd = commands.get(currentFrame);
  vkResetCommandBuffer(cmd, 0);

  camera.update(currentFrame);

  recorder.record(cmd, swapchain, imageIndex, currentFrame, items, camera);

  VkPipelineStageFlags waitStage =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &frame.getImageAvailableSemaphore(currentFrame);
  submit.pWaitDstStageMask = &waitStage;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cmd;
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &frame.getRenderFinishedSemaphore(imageIndex);

  VK_CHECK(vkQueueSubmit(device.getGraphicsQueue(), 1, &submit, fence));

  VkPresentInfoKHR present{};
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = &frame.getRenderFinishedSemaphore(imageIndex);
  present.swapchainCount = 1;
  VkSwapchainKHR sc = swapchain.getSwapchain();
  present.pSwapchains = &sc;
  present.pImageIndices = &imageIndex;

  res = vkQueuePresentKHR(device.getPresentQueue(), &present);

  currentFrame = (currentFrame + 1) % frame.getMaxFramesInFlight();

  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
    return RenderResult::SwapchainOutOfDate;

  return res == VK_SUCCESS ? RenderResult::Ok : RenderResult::FatalError;
}
