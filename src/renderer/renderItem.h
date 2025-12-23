#pragma once
#include "backend/buffer.h"
#include <glm/glm.hpp>

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
  const Mesh *mesh;
  Material *material;
  glm::mat4 transform;
};
