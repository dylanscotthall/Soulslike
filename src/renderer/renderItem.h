#pragma once
#include "backend/buffer.h"
#include "descriptor.h"
#include <glm/glm.hpp>
#include <memory>

struct Mesh {
  Buffer vertexBuffer;
  Buffer indexBuffer;
  uint32_t indexCount;

  Mesh(Device &device, VkCommandPool commandPool)
      : vertexBuffer(device, commandPool), indexBuffer(device, commandPool),
        indexCount(0) {}
};

struct Material {};

struct RenderItem {
  const Mesh *mesh = nullptr;
  Material *material = nullptr;
  glm::mat4 transform = glm::mat4(1.0f);
  std::unique_ptr<DescriptorSet> descriptorSet;
  Buffer modelBuffer;

  RenderItem(Device &device, VkCommandPool pool) : modelBuffer(device, pool) {}

  // Delete everything else
  RenderItem(const RenderItem &) = delete;
  RenderItem &operator=(const RenderItem &) = delete;
  RenderItem(RenderItem &&) = delete;
  RenderItem &operator=(RenderItem &&) = delete;
};
