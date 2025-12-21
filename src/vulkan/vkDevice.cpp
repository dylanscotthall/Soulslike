#include "vkDevice.h"
#include "vkSwapchain.h"
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

QueueFamilyIndices vDevice::findQueueFamilies(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }
    if (indices.isComplete()) {
      break;
    }

    i++;
  }
  return indices;
}

bool vDevice::checkDeviceExtensionsSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }
  return requiredExtensions.empty();
}

bool vDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
  QueueFamilyIndices indices = findQueueFamilies(device, surface);

  bool extensionsSupported = checkDeviceExtensionsSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapchainSupportDetails swapChainSupport =
        vSwapchain::querySwapchainSupport(device, surface);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

vDevice::vDevice(const vInstance &instance, VkSurfaceKHR surface,
                 bool enableValidationLayers) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan Support!");
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount,
                             devices.data());
  for (const auto &device : devices) {
    if (isDeviceSuitable(device, surface)) {
      physicalDevice = device;
      break;
    }
  }
  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU");
  }
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(instance.getValidationLayers().size());
    createInfo.ppEnabledLayerNames = instance.getValidationLayers().data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

vDevice::~vDevice() { vkDestroyDevice(device, nullptr); }

VkDevice vDevice::getLogical() const noexcept { return device; }
VkPhysicalDevice vDevice::getPhysical() const noexcept {
  return physicalDevice;
};
QueueFamilyIndices &vDevice::getQueues() noexcept { return queueFamilies; }
VkQueue vDevice::getGraphicsQueue() const noexcept { return graphicsQueue; }
VkQueue vDevice::getPresentQueue() const noexcept { return presentQueue; }
