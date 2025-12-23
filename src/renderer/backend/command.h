#pragma once

#include "../renderItem.h"
#include "pipeline.h"
#include "swapchain.h"
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
class Command {
public:
  Command(VkPhysicalDevice physicalDevice, VkDevice device,
          VkSurfaceKHR surface, Pipeline &pipeline);
  ~Command();

  void createCommandBuffers(VkDevice device, const int MAX_FRAMES_IN_FLIGHT);

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           Swapchain &swapchain,
                           std::span<const RenderItem> items);

  VkCommandPool getCommandPool() const noexcept;
  std::vector<VkCommandBuffer> &getCommandBuffers() noexcept;

private:
  VkDevice device;
  VkCommandPool commandPool;
  Pipeline &pipeline;
  std::vector<VkCommandBuffer> commandBuffers;
};
