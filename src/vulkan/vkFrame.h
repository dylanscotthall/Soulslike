#pragma once

#include "vkCommand.h"
#include "vkSwapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class vFrame {
public:
  vFrame(vDevice &device);
  ~vFrame();

  void drawFrame(uint32_t currentFrame, vSwapchain &swapchain,
                 VkSurfaceKHR surface, VkRenderPass renderPass, Window &window,
                 VkBuffer vertexBuffer, vCommand &command, vPipeline &pipeline);

  std::vector<VkSemaphore> getImageAvailableSemaphores() const noexcept;
  std::vector<VkSemaphore> getRenderFinishedSemaphores() const noexcept;
  std::vector<VkFence> getInFlightFences() const noexcept;
  const int getMaxFramesInFlight() const noexcept;

private:
  vDevice &device;
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  const int MAX_FRAMES_IN_FLIGHT = 2;
};
