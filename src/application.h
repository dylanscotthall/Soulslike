#pragma once
#include "vulkan/surface.h"
#include "vulkan/vkBuffer.h"
#include "vulkan/vkCommand.h"
#include "vulkan/vkDevice.h"
#include "vulkan/vkFrame.h"
#include "vulkan/vkInstance.h"
#include "vulkan/vkPipeline.h"
#include "vulkan/vkSwapchain.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Application {
public:
  void run() { mainLoop(); }

  Application();
  ~Application();

private:
  void mainLoop();

private:
  void initVulkan();

private:
  Window window;
  vInstance instance;
  Surface surface;
  vDevice device;
  vSwapchain swapchain;
  vPipeline pipeline;
  vBuffer vertexBuffer;
  vCommand command;
  vFrame frame;
  uint32_t currentFrame = 0;
};
