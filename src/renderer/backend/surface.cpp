#include "surface.h"
#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Surface::Surface(VkInstance instance, const Window &window)
    : instance(instance) {
  if (glfwCreateWindowSurface(instance, window.getWindow(), nullptr,
                              &surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

Surface::~Surface() { vkDestroySurfaceKHR(instance, surface, nullptr); }
