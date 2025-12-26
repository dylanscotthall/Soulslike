#pragma once
#include "renderer/uniforms.h"
#include "rhi/vulkan/buffer.h"
#include "rhi/vulkan/device.h"
#include <glm/glm.hpp>
#include <memory>

class Camera {
public:
  Camera(Device &device, VkCommandPool commandPool, uint32_t framesInFlight);

  void setPerspective(float fov, float aspect, float near, float far);
  void setPosition(const glm::vec3 &pos);
  void lookAt(const glm::vec3 &target, const glm::vec3 &up);
  void update(uint32_t frameIndex);

  VkBuffer getBuffer() const { return buffer->get(); }

private:
  Device &device;
  VkCommandPool commandPool;
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  CameraUBO ubo{};

  std::unique_ptr<Buffer> buffer;
};
