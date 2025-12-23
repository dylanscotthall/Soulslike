#include "buffer.h"
#include <cstring>
#include <stdexcept>

Buffer::Buffer(Device &device, VkCommandPool commandPool)
    : device(device), commandPool(commandPool) {}

Buffer::~Buffer() {
  if (buffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(device.getLogical(), buffer, nullptr);
  }
  if (memory != VK_NULL_HANDLE) {
    vkFreeMemory(device.getLogical(), memory, nullptr);
  }
}

void Buffer::create(VkDeviceSize bufferSize, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties) {
  size = bufferSize;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = bufferSize;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device.getLogical(), &bufferInfo, nullptr, &buffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create buffer");
  }

  VkMemoryRequirements memReq;
  vkGetBufferMemoryRequirements(device.getLogical(), buffer, &memReq);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memReq.size;
  allocInfo.memoryTypeIndex =
      device.findMemoryType(memReq.memoryTypeBits, properties);

  if (vkAllocateMemory(device.getLogical(), &allocInfo, nullptr, &memory) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate buffer memory");
  }

  vkBindBufferMemory(device.getLogical(), buffer, memory, 0);
}

void Buffer::uploadViaStaging(const void *srcData, VkDeviceSize dataSize) {
  // staging buffer
  Buffer staging(device, commandPool);
  staging.create(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  void *mapped;
  vkMapMemory(device.getLogical(), staging.memory, 0, dataSize, 0, &mapped);
  std::memcpy(mapped, srcData, static_cast<size_t>(dataSize));
  vkUnmapMemory(device.getLogical(), staging.memory);

  copyBuffer(staging.buffer, buffer, dataSize);
}

void Buffer::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize copySize) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer cmd;
  vkAllocateCommandBuffers(device.getLogical(), &allocInfo, &cmd);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(cmd, &beginInfo);

  VkBufferCopy copy{};
  copy.srcOffset = 0;
  copy.dstOffset = 0;
  copy.size = copySize;

  vkCmdCopyBuffer(cmd, src, dst, 1, &copy);

  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cmd;

  vkQueueSubmit(device.getGraphicsQueue(), 1, &submit, VK_NULL_HANDLE);
  vkQueueWaitIdle(device.getGraphicsQueue());

  vkFreeCommandBuffers(device.getLogical(), commandPool, 1, &cmd);
}
