#pragma once
#include "renderer/uniforms.h"
#include "rhi/vulkan/buffer.h"
#include "rhi/vulkan/descriptor.h"
#include <glm/glm.hpp>
#include <memory>

struct Mesh {
  Buffer vertexBuffer;
  Buffer indexBuffer;
  uint32_t indexCount;

  Mesh(Device &device, VkCommandPool pool)
      : vertexBuffer(device, pool), indexBuffer(device, pool), indexCount(0) {}
};

struct Material {};

struct RenderItem {
  const Mesh *mesh = nullptr;
  Material *material = nullptr;
  glm::mat4 transform = glm::mat4(1.0f);

  Buffer modelBuffer;
  std::unique_ptr<DescriptorSet> descriptorSet;

  RenderItem(Device &device, VkCommandPool pool) : modelBuffer(device, pool) {}

  void init(Device &device, VkDescriptorSetLayout layout,
            uint32_t framesInFlight, VkBuffer cameraBuffer,
            VkDeviceSize cameraSize) {
    modelBuffer.create(sizeof(ModelUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    descriptorSet =
        std::make_unique<DescriptorSet>(device, layout, framesInFlight);

    for (uint32_t i = 0; i < framesInFlight; ++i) {
      descriptorSet->update(i, cameraBuffer, cameraSize, modelBuffer.get(),
                            sizeof(ModelUBO));
    }
  }

  void update(uint32_t frameIndex, VkBuffer cameraBuffer,
              VkDeviceSize cameraSize) {
    ModelUBO ubo{};
    ubo.model = transform;
    modelBuffer.upload(&ubo, sizeof(ModelUBO));

    descriptorSet->update(frameIndex, cameraBuffer, cameraSize,
                          modelBuffer.get(), sizeof(ModelUBO));
  }

  RenderItem(const RenderItem &) = delete;
  RenderItem &operator=(const RenderItem &) = delete;
};
