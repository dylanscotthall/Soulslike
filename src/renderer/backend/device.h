#pragma once
#include "instance.h"
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class Device {
public:
  Device(const Instance &instance, VkSurfaceKHR surface,
         bool enableValidationLayers);
  ~Device();

  bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
  static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  bool checkDeviceExtensionsSupport(VkPhysicalDevice device);

  VkDevice getLogical() const noexcept;
  VkPhysicalDevice getPhysical() const noexcept;
  QueueFamilyIndices &getQueues() noexcept;
  VkQueue getGraphicsQueue() const noexcept;
  VkQueue getPresentQueue() const noexcept;

private:
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  QueueFamilyIndices queueFamilies;
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
