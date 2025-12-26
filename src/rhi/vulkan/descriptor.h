#pragma once
#include "rhi/vulkan/device.h"
#include <vector>
#include <vulkan/vulkan_core.h>

class DescriptorSet {
public:
  DescriptorSet(Device &device, VkDescriptorSetLayout layout,
                uint32_t framesInFlight);
  ~DescriptorSet();

  void update(uint32_t frameIndex, VkBuffer cameraBuffer,
              VkDeviceSize cameraSize, VkBuffer modelBuffer,
              VkDeviceSize modelSize);

  VkDescriptorSet &get(uint32_t frameIndex);

private:
  Device &device;
  VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> descriptorSets;
};
