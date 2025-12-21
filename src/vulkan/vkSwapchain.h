#pragma once
#include "vkDevice.h"
#include "window.h"
#include <vector>
#include <vulkan/vulkan_core.h>

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class vSwapchain {
public:
  vSwapchain(vDevice &device, VkSurfaceKHR surface, Window &window);
  ~vSwapchain();

  static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface);

  void createSwapchain(VkSurfaceKHR surface, Window &window);
  void recreateSwapchain(VkSurfaceKHR surface, Window &window,
                         VkRenderPass renderPass);
  void cleanupSwapChain();

  void createImageViews();
  void createFramebuffers(VkRenderPass renderPass);

  VkSwapchainKHR getSwapchain() const noexcept;
  std::vector<VkImage> getSwapchainImages() const noexcept;
  VkFormat getSwapchainImageFormat() const noexcept;
  VkExtent2D getSwapchainExtent() const noexcept;
  std::vector<VkImageView> getSwapchainImageViews() const noexcept;
  std::vector<VkFramebuffer> getSwapchainFramebuffers() const noexcept;

  void resizeSwapchainImageViewsToSwapchainImages() {
    swapchainImageViews.resize(swapchainImages.size());
  }
  void resizeSwapchainFrameBuffersToSwapchainImageViews() {
    swapchainFramebuffers.resize(swapchainImageViews.size());
  }

private:
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImages;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViews;
  std::vector<VkFramebuffer> swapchainFramebuffers;
};
