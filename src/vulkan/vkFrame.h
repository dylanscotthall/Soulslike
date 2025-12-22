#pragma once

#include "vkCommand.h"
#include "vkSwapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>

class vFrame {
public:
  vFrame(vDevice &device, VkSwapchainKHR swapchain, int maxFramesInFlight = 2);
  ~vFrame();

  void drawFrame(uint32_t &currentFrame, vSwapchain &swapchain,
                 VkSurfaceKHR surface, Window &window, VkBuffer vertexBuffer,
                 VkBuffer indexBuffer, vCommand &command, vPipeline &pipeline);

  int getMaxFramesInFlight() const noexcept { return MAX_FRAMES_IN_FLIGHT; }

private:
  vDevice &device;

  int MAX_FRAMES_IN_FLIGHT;

  std::vector<VkFence> inFlightFences;               // one per frame in flight
  std::vector<VkFence> imagesInFlight;               // tracks per-image fence
  std::vector<VkSemaphore> renderFinishedSemaphores; // per-image
  std::vector<VkSemaphore> imageAvailableSemaphores; // per-image
};
