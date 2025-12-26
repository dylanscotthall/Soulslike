#pragma once
#include "renderer/camera.h"
#include "rhi/vulkan/frame.h"
#include <span>

class Device;
class Swapchain;
class CommandContext;
class RenderRecorder;
class RenderItem;

enum class RenderResult { Ok, SwapchainOutOfDate, FatalError };

class Renderer {
public:
  Renderer(Device &device, Swapchain &swapchain, CommandContext &commands,
           RenderRecorder &recorder, Frame &frame);

  RenderResult drawFrame(std::span<RenderItem *> items, Camera &camera);

  const uint32_t &getCurrentFrame() const noexcept { return currentFrame; }

private:
  Device &device;
  Swapchain &swapchain;
  CommandContext &commands;
  RenderRecorder &recorder;
  Frame &frame;

  uint32_t currentFrame = 0;
};
