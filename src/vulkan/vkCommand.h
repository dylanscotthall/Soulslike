#pragma once

#include "vkPipeline.h"
#include "vkSwapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class vCommand {
public:
  vCommand(VkPhysicalDevice physicalDevice, VkDevice device,
           VkSurfaceKHR surface);
  ~vCommand();

  void createCommandBuffers(VkDevice device, const int MAX_FRAMES_IN_FLIGHT);

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           vSwapchain &swapchain, vPipeline &pipeline,
                           VkBuffer vertexBuffer, VkBuffer indexBuffer);

  VkCommandPool getCommandPool() const noexcept;
  std::vector<VkCommandBuffer> &getCommandBuffers() noexcept;

private:
  VkDevice device;
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;
};
