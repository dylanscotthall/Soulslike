#include "rhi/vulkan/surface.h"
#include "helper.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Surface::Surface(VkInstance instance, const Window &window)
    : instance(instance) {
  VK_CHECK(
      glfwCreateWindowSurface(instance, window.getWindow(), nullptr, &surface));
}

Surface::~Surface() { vkDestroySurfaceKHR(instance, surface, nullptr); }
