#pragma once

#include "backend/device.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class DescriptorSet {
public:
  DescriptorSet(Device &device, VkDescriptorSetLayout layout,
                uint32_t maxFrames);
  ~DescriptorSet();

  void update(uint32_t currentFrame, VkBuffer uniformBuffer,
              VkDeviceSize bufferSize);
  VkDescriptorSet &get(uint32_t frame);

private:
  Device &device;
  VkDescriptorPool descriptorPool;
  VkDescriptorSetLayout layout;
  std::vector<VkDescriptorSet> descriptorSets;
};
