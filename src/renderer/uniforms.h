#pragma once
#include <glm/glm.hpp>

struct CameraUBO {
  glm::mat4 view;
  glm::mat4 proj;
};

struct ModelUBO {
  glm::mat4 model;
};
