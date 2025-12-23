#pragma once
#include "backend/buffer.h"
#include "backend/command.h"
#include "backend/device.h"
#include "backend/frame.h"
#include "backend/swapchain.h"
#include "renderItem.h"
#include <span>

enum class RenderResult { Ok, SwapchainOutOfDate, FatalError };

class Renderer {
public:
  Renderer(Device &device, Swapchain &swapchain, Command &command,
           Frame &frame);
  ~Renderer();

  RenderResult drawFrame(std::span<const RenderItem> items);

private:
  Device &device;
  Swapchain &swapchain;
  Command &command;
  Frame &frame;

  uint32_t currentFrame = 0;
};
