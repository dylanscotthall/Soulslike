#pragma once

#include "command.h"
#include "swapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>

class Frame {
public:
  Frame(Device &device, VkSwapchainKHR swapchain, int maxFramesInFlight = 2);
  ~Frame();

  int getMaxFramesInFlight() const noexcept { return MAX_FRAMES_IN_FLIGHT; }

  // read-only access
  // const std::vector<VkFence> &getInFlightFences() const;
  // const std::vector<VkFence> &getImagesInFlight() const;
  // const std::vector<VkSemaphore> &getRenderFinishedSemaphores() const;
  // const std::vector<VkSemaphore> &getImageAvailableSemaphores() const;

  const VkFence &getInFlightFence(uint32_t index) const;
  const VkFence &getImageInFlight(uint32_t index) const;
  const VkSemaphore &getRenderFinishedSemaphore(uint32_t index) const;
  const VkSemaphore &getImageAvailableSemaphore(uint32_t index) const;

  void setImagesInFlight(uint32_t imageIndex, VkFence fence);

private:
  Device &device;

  int MAX_FRAMES_IN_FLIGHT;

  std::vector<VkFence> inFlightFences;               // one per frame in flight
  std::vector<VkFence> imagesInFlight;               // tracks per-image fence
  std::vector<VkSemaphore> renderFinishedSemaphores; // per-image
  std::vector<VkSemaphore> imageAvailableSemaphores; // per-image
};
