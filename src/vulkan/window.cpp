#include "window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

static void framebufferResizeCallback(GLFWwindow *window, int width,
                                      int height) {
  auto win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  win->setFrameBufferResized(true);
}

bool Window::getFrameBufferResized() const noexcept {
  return framebufferResized;
}
void Window::setFrameBufferResized(bool value) noexcept {
  framebufferResized = value;
}

std::pair<int, int> Window::framebufferSize() const noexcept {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  return {w, h};
}

bool Window::shouldClose() const noexcept {
  return glfwWindowShouldClose(window);
}

GLFWwindow *Window::getWindow() const noexcept { return window; }

Window::Window(std::string_view title, uint32_t w, uint32_t h) {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");

  glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
  // glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(w, h, title.data(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  if (!window)
    throw std::runtime_error("Failed to create GLFW window");

  glfwShowWindow(window);
}

void Window::pollEvents() const noexcept { glfwPollEvents(); }
void Window::waitEvents() const noexcept { glfwWaitEvents(); }

std::vector<const char *>
Window::getRequiredExtensions(bool enableValidationLayers) const {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
