#include "renderer/camera.h"
#include "../helper.h"
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(Device &device, VkCommandPool commandPool,
               uint32_t framesInFlight)
    : device(device), commandPool(commandPool) {
  buffer = std::make_unique<Buffer>(device, commandPool);
  buffer->create(sizeof(CameraUBO) * framesInFlight,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  ubo.view = glm::mat4(1.0f);
  ubo.proj = glm::mat4(1.0f);
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
  ubo.proj = glm::perspective(glm::radians(fov), aspect, near, far);
  ubo.proj[1][1] *= -1; // Vulkan Y flip
}

void Camera::setPosition(const glm::vec3 &pos) { position = pos; }

void Camera::lookAt(const glm::vec3 &target, const glm::vec3 &up) {
  ubo.view = glm::lookAt(position, target, up);
}

void Camera::update(uint32_t frameIndex) {
  VkDeviceSize offset = frameIndex * sizeof(CameraUBO);
  void *data = nullptr;
  vkMapMemory(device.getLogical(), buffer->getMemory(), offset,
              sizeof(CameraUBO), 0, &data);
  std::memcpy(data, &ubo, sizeof(CameraUBO));
  vkUnmapMemory(device.getLogical(), buffer->getMemory());
}
