#include "swapchain.h"
#include "../../helper.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

SwapchainSupportDetails
Swapchain::querySwapchainSupport(VkPhysicalDevice device,
                                 VkSurfaceKHR surface) {
  SwapchainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }
  return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                            Window &window) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    auto [width, height] = window.framebufferSize();

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void Swapchain::createSwapchain(VkSurfaceKHR surface, Window &window) {
  SwapchainSupportDetails swapChainSupport =
      querySwapchainSupport(device.getPhysical(), surface);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

  swapchainImageFormat = surfaceFormat.format;
  swapchainExtent = extent;

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices =
      Device::findQueueFamilies(device.getPhysical(), surface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VK_CHECK(vkCreateSwapchainKHR(device.getLogical(), &createInfo, nullptr,
                                &swapchain));

  vkGetSwapchainImagesKHR(device.getLogical(), swapchain, &imageCount, nullptr);
  swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device.getLogical(), swapchain, &imageCount,
                          swapchainImages.data());
}

void Swapchain::recreateSwapchain(VkSurfaceKHR surface, Window &window) {
  auto [width, height] = window.framebufferSize();
  while (width == 0 || height == 0) {
    std::tie(width, height) = window.framebufferSize();
    window.waitEvents();
  }
  vkDeviceWaitIdle(device.getLogical());

  cleanupSwapChain();

  createSwapchain(surface, window);
  createImageViews();
  createDepthResources();
}

void Swapchain::createImageViews() {
  Swapchain::resizeSwapchainImageViewsToSwapchainImages();

  for (size_t i = 0; i < swapchainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapchainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapchainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device.getLogical(), &createInfo, nullptr,
                               &swapchainImageViews[i]));
  }
}
void Swapchain::createDepthResources() {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = swapchainExtent.width;
  imageInfo.extent.height = swapchainExtent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = depthFormat;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VK_CHECK(
      vkCreateImage(device.getLogical(), &imageInfo, nullptr, &depthImage));

  VkMemoryRequirements memReq;
  vkGetImageMemoryRequirements(device.getLogical(), depthImage, &memReq);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memReq.size;
  allocInfo.memoryTypeIndex = device.findMemoryType(
      memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VK_CHECK(
      vkAllocateMemory(device.getLogical(), &allocInfo, nullptr, &depthMemory));

  vkBindImageMemory(device.getLogical(), depthImage, depthMemory, 0);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = depthImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = depthFormat;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VK_CHECK(vkCreateImageView(device.getLogical(), &viewInfo, nullptr,
                             &depthImageView));
}

Swapchain::~Swapchain() {}

void Swapchain::cleanupSwapChain() {
  for (auto imageView : swapchainImageViews) {
    vkDestroyImageView(device.getLogical(), imageView, nullptr);
  }

  vkDestroySwapchainKHR(device.getLogical(), swapchain, nullptr);
  vkDestroyImageView(device.getLogical(), depthImageView, nullptr);
  vkDestroyImage(device.getLogical(), depthImage, nullptr);
  vkFreeMemory(device.getLogical(), depthMemory, nullptr);
}

Swapchain::Swapchain(Device &device, VkSurfaceKHR surface, Window &window)
    : device(device) {
  createSwapchain(surface, window);
  createImageViews();
  createDepthResources();
}

VkSwapchainKHR Swapchain::getSwapchain() const noexcept { return swapchain; }
std::vector<VkImage> Swapchain::getSwapchainImages() const noexcept {
  return swapchainImages;
}
VkFormat Swapchain::getSwapchainImageFormat() const noexcept {
  return swapchainImageFormat;
}
VkExtent2D Swapchain::getSwapchainExtent() const noexcept {
  return swapchainExtent;
}
std::vector<VkImageView> Swapchain::getSwapchainImageViews() const noexcept {
  return swapchainImageViews;
}
VkImageView Swapchain::getDepthImageView() const noexcept {
  return depthImageView;
}

VkImage Swapchain::getDepthImage() const noexcept { return depthImage; }
VkFormat Swapchain::getDepthFormat() const noexcept { return depthFormat; }
