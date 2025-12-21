#pragma once

#include "vkPipeline.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class vBuffer {
public:
  vBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
          std::vector<Vertex> vertices);
  ~vBuffer();

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  VkBuffer get() const noexcept;

private:
  VkDevice device;
  VkPhysicalDevice physicalDevice;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
};
