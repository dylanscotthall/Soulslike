#pragma once

#include "vkDevice.h"
#include <vulkan/vulkan_core.h>
class vRenderPass {
public:
  vRenderPass(VkFormat swapchainImageFormat, VkDevice device);
  ~vRenderPass();

  VkRenderPass get() const noexcept;

private:
  VkRenderPass renderPass;
  VkDevice device;
};
