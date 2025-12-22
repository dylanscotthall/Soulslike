#pragma once

#include "vkDevice.h"
#include <vulkan/vulkan.h>

class vBuffer {
public:
  vBuffer(vDevice &device, VkCommandPool commandPool);
  ~vBuffer();

  // non-copyable
  vBuffer(const vBuffer &) = delete;
  vBuffer &operator=(const vBuffer &) = delete;

  void create(VkDeviceSize size, VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties);

  void uploadViaStaging(const void *data, VkDeviceSize size);

  VkBuffer get() const noexcept { return buffer; }
  VkDeviceSize getSize() const noexcept { return size; }

private:
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

private:
  VkCommandPool commandPool;
  vDevice &device;
  VkBuffer buffer{VK_NULL_HANDLE};
  VkDeviceMemory memory{VK_NULL_HANDLE};
  VkDeviceSize size{0};
};
