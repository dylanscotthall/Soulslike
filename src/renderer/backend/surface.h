#pragma once
#include "window.h"
#include <vulkan/vulkan_core.h>
class Surface {
public:
  Surface(VkInstance instance, const Window &window);
  ~Surface();

  VkSurfaceKHR get() const noexcept { return surface; }

private:
  VkInstance instance;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
};
