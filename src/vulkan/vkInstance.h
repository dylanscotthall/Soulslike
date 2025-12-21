#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

class vInstance {
public:
  vInstance(bool enableValidationLayers);
  ~vInstance();

private:
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

public:
  bool checkValidationLayerSupport();
  bool enableValidationLayers;
  void setupDebugMessenger();

  const std::vector<const char *> &getValidationLayers() const;
  VkInstance getInstance() const noexcept;
  VkDebugUtilsMessengerEXT getDebugMessenger() const noexcept;
};
