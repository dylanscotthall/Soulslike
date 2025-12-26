#include "rhi/vulkan/commandContext.h"
#include "helper.h"
#include "rhi/vulkan/device.h"
#include <vulkan/vulkan_core.h>

CommandContext::CommandContext(VkPhysicalDevice phys, VkDevice device,
                               VkSurfaceKHR surface)
    : device(device) {

  auto indices = Device::findQueueFamilies(phys, surface);

  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = indices.graphicsFamily.value();

  VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &pool));
}

void CommandContext::allocate(uint32_t framesInFlight) {
  buffers.resize(framesInFlight);

  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandPool = pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandBufferCount = framesInFlight;

  VK_CHECK(vkAllocateCommandBuffers(device, &info, buffers.data()));
}

VkCommandBuffer CommandContext::get(uint32_t frame) const noexcept {
  return buffers[frame];
}

VkCommandPool CommandContext::getPool() const noexcept { return pool; }

CommandContext::~CommandContext() {
  vkDeviceWaitIdle(device);
  vkDestroyCommandPool(device, pool, nullptr);
}
