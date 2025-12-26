#pragma once
#include <stdexcept>
#define VK_CHECK(x)                                                            \
  if ((x) != VK_SUCCESS)                                                       \
    throw std::runtime_error("Vulkan call failed: " #x);
