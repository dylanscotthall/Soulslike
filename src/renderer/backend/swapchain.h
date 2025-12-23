#pragma once
#include "device.h"
#include "window.h"
#include <vector>
#include <vulkan/vulkan_core.h>

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
public:
  Swapchain(Device &device, VkSurfaceKHR surface, Window &window);
  ~Swapchain();

  static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface);

  void createSwapchain(VkSurfaceKHR surface, Window &window);
  void recreateSwapchain(VkSurfaceKHR surface, Window &window);
  void cleanupSwapChain();

  void createImageViews();
  void createDepthResources();

  VkSwapchainKHR getSwapchain() const noexcept;
  std::vector<VkImage> getSwapchainImages() const noexcept;
  VkFormat getSwapchainImageFormat() const noexcept;
  VkExtent2D getSwapchainExtent() const noexcept;
  std::vector<VkImageView> getSwapchainImageViews() const noexcept;
  std::vector<VkFramebuffer> getSwapchainFramebuffers() const noexcept;
  VkImageView getDepthImageView() const noexcept;
  VkImage getDepthImage() const noexcept;
  VkFormat getDepthFormat() const noexcept;

  void resizeSwapchainImageViewsToSwapchainImages() {
    swapchainImageViews.resize(swapchainImages.size());
  }

private:
  Device &device;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImages;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViews;

  // Depth Shit
  VkImage depthImage = VK_NULL_HANDLE;
  VkDeviceMemory depthMemory = VK_NULL_HANDLE;
  VkImageView depthImageView = VK_NULL_HANDLE;
  VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
};
