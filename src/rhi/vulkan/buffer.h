#pragma once
#include "rhi/vulkan/device.h"
#include <vulkan/vulkan_core.h>

class Buffer {
public:
  Buffer(Device &device, VkCommandPool commandPool);
  ~Buffer();

  void create(VkDeviceSize bufferSize, VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties);
  void upload(const void *data, VkDeviceSize dataSize);
  void uploadViaStaging(const void *srcData, VkDeviceSize dataSize);
  void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize copySize);
  void createUniformBuffer(VkDeviceSize size);

  VkBuffer get() const { return buffer; }
  VkDeviceMemory getMemory() const { return memory; }

private:
  Device &device;
  VkCommandPool commandPool;
  VkBuffer buffer = VK_NULL_HANDLE;
  VkDeviceMemory memory = VK_NULL_HANDLE;
  VkDeviceSize size = 0;
};
