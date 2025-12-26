#pragma once
#include <string_view>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
  Window(std::string_view title, uint32_t w, uint32_t h);
  ~Window();

private:
  GLFWwindow *window = nullptr;
  bool framebufferResized = false;

public:
  GLFWwindow *getWindow() const noexcept;
  bool shouldClose() const noexcept;
  void pollEvents() const noexcept;
  void waitEvents() const noexcept;

  float getAspectRatio() const noexcept;

  std::vector<const char *>
  getRequiredExtensions(bool enabledValidationLayer) const;
  std::pair<int, int> framebufferSize() const noexcept;
  void setFrameBufferResized(bool value) noexcept;
  bool getFrameBufferResized() const noexcept;
};
