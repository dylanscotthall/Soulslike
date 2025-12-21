#pragma once

#include <vulkan/vulkan_core.h>
class vBuffer {
public:
  vBuffer(VkDevice device, VkPhysicalDevice physicalDevice);
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
