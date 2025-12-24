#pragma once

#include "device.h"
#include <vulkan/vulkan.h>

class Buffer {
public:
  Buffer(Device &device, VkCommandPool commandPool);
  ~Buffer();

  // non-copyable
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  void create(VkDeviceSize size, VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties);

  void uploadViaStaging(const void *data, VkDeviceSize size);

  VkBuffer get() const noexcept { return buffer; }
  VkDeviceSize getSize() const noexcept { return size; }
  VkDeviceMemory getMemory() const;

private:
  void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
  void createUniformBuffer(VkDeviceSize size);

private:
  VkCommandPool commandPool;
  Device &device;
  VkBuffer buffer{VK_NULL_HANDLE};
  VkDeviceMemory memory{VK_NULL_HANDLE};
  VkDeviceSize size{0};
};
