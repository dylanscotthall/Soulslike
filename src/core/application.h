#pragma once
#include "renderer/camera.h"
#include "renderer/renderItem.h"
#include "renderer/renderer.h"
#include "rhi/vulkan/buffer.h"
#include "rhi/vulkan/commandContext.h"
#include "rhi/vulkan/device.h"
#include "rhi/vulkan/frame.h"
#include "rhi/vulkan/instance.h"
#include "rhi/vulkan/pipeline.h"
#include "rhi/vulkan/renderRecorder.h"
#include "rhi/vulkan/surface.h"
#include "rhi/vulkan/swapchain.h"
#include <cstdint>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
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
  CommandContext commandContext;
  Frame frame;
  RenderRecorder recorder;
  Renderer renderer;
  std::vector<std::unique_ptr<Mesh>> meshes;
  std::vector<std::unique_ptr<RenderItem>> renderItems;
  std::unique_ptr<Camera> camera;
};
