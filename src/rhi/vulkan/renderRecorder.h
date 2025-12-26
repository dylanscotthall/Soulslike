#pragma once
#include "renderer/camera.h"
#include "renderer/renderItem.h"
#include "rhi/vulkan/pipeline.h"
#include "rhi/vulkan/swapchain.h"
#include <span>
#include <vulkan/vulkan_core.h>

class RenderRecorder {
public:
  RenderRecorder(Pipeline &pipeline);

  void record(VkCommandBuffer cmd, Swapchain &swapchain, uint32_t imageIndex,
              uint32_t frame, std::span<RenderItem *> items, Camera &camera);

private:
  Pipeline &pipeline;
};
