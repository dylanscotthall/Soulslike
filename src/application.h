#pragma once
#include "renderer/backend/buffer.h"
#include "renderer/backend/command.h"
#include "renderer/backend/device.h"
#include "renderer/backend/frame.h"
#include "renderer/backend/instance.h"
#include "renderer/backend/pipeline.h"
#include "renderer/backend/surface.h"
#include "renderer/backend/swapchain.h"
#include "renderer/renderer.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
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
  Instance instance;
  Surface surface;
  Device device;
  Swapchain swapchain;
  Pipeline pipeline;
  Command command;
  Frame frame;
  Renderer renderer;
  std::vector<std::unique_ptr<Mesh>> meshes;
  std::vector<std::unique_ptr<RenderItem>> renderItems;
};
