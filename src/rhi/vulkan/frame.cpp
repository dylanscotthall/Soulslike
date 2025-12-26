#include "rhi/vulkan/frame.h"
#include "helper.h"
#include <vulkan/vulkan_core.h>

Frame::Frame(Device &device, VkSwapchainKHR swapchain, int maxFramesInFlight)
    : device(device), MAX_FRAMES_IN_FLIGHT(maxFramesInFlight) {

  uint32_t imageCount = 0;
  vkGetSwapchainImagesKHR(device.getLogical(), swapchain, &imageCount, nullptr);
  imagesInFlight.resize(imageCount, VK_NULL_HANDLE);
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(imageCount);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VK_CHECK(vkCreateFence(device.getLogical(), &fenceInfo, nullptr,
                           &inFlightFences[i]));
  }

  // image-available: per-frame
  for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
    VK_CHECK(vkCreateSemaphore(device.getLogical(), &semaphoreInfo, nullptr,
                               &imageAvailableSemaphores[i]));
  }

  // render-finished: per-image
  for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
    VK_CHECK(vkCreateSemaphore(device.getLogical(), &semaphoreInfo, nullptr,
                               &renderFinishedSemaphores[i]));
  }
}

const VkFence &Frame::getInFlightFence(uint32_t index) const {
  return inFlightFences[index];
}
VkFence &Frame::getImageInFlight(uint32_t index) {
  return imagesInFlight[index];
}
const VkSemaphore &Frame::getRenderFinishedSemaphore(uint32_t index) const {
  return renderFinishedSemaphores[index];
}
const VkSemaphore &Frame::getImageAvailableSemaphore(uint32_t index) const {
  return imageAvailableSemaphores[index];
}

void Frame::setImageInFlight(uint32_t imageIndex, VkFence fence) {
  imagesInFlight[imageIndex] = fence;
}
Frame::~Frame() {
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
