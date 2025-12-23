#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

class Instance {
public:
  Instance(bool enableValidationLayers);
  ~Instance();

  const std::vector<const char *> &getValidationLayers() const;
  VkInstance getInstance() const noexcept;
  VkDebugUtilsMessengerEXT getDebugMessenger() const noexcept;

private:
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  bool checkValidationLayerSupport();
  bool enableValidationLayers;
  void setupDebugMessenger();
};
