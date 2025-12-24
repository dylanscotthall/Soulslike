#include "descriptor.h"
#include "../helper.h"
#include <stdexcept>

DescriptorSet::DescriptorSet(Device &device, VkDescriptorSetLayout layout,
                             uint32_t maxFrames)
    : device(device), layout(layout) {

  std::vector<VkDescriptorPoolSize> poolSizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxFrames},
  };

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = maxFrames;

  VK_CHECK(vkCreateDescriptorPool(device.getLogical(), &poolInfo, nullptr,
                                  &descriptorPool));

  std::vector<VkDescriptorSetLayout> layouts(maxFrames, layout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = maxFrames;
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(maxFrames);
  VK_CHECK(vkAllocateDescriptorSets(device.getLogical(), &allocInfo,
                                    descriptorSets.data()));
}

DescriptorSet::~DescriptorSet() {
  vkDestroyDescriptorPool(device.getLogical(), descriptorPool, nullptr);
}

void DescriptorSet::update(uint32_t currentFrame, VkBuffer uniformBuffer,
                           VkDeviceSize bufferSize) {
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = uniformBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range = bufferSize;

  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = descriptorSets[currentFrame];
  descriptorWrite.dstBinding = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo = &bufferInfo;

  vkUpdateDescriptorSets(device.getLogical(), 1, &descriptorWrite, 0, nullptr);
}

VkDescriptorSet &DescriptorSet::get(uint32_t frame) {
  return descriptorSets[frame];
}
