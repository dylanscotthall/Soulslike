#include "rhi/vulkan/descriptor.h"
#include "helper.h"

DescriptorSet::DescriptorSet(Device &device, VkDescriptorSetLayout layout,
                             uint32_t framesInFlight)
    : device(device) {
  std::vector<VkDescriptorPoolSize> poolSizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 * framesInFlight}};

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = framesInFlight;

  VK_CHECK(vkCreateDescriptorPool(device.getLogical(), &poolInfo, nullptr,
                                  &descriptorPool));

  std::vector<VkDescriptorSetLayout> layouts(framesInFlight, layout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = framesInFlight;
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(framesInFlight);
  VK_CHECK(vkAllocateDescriptorSets(device.getLogical(), &allocInfo,
                                    descriptorSets.data()));
}

DescriptorSet::~DescriptorSet() {
  if (descriptorPool != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(device.getLogical(), descriptorPool, nullptr);
  }
}

void DescriptorSet::update(uint32_t frameIndex, VkBuffer cameraBuffer,
                           VkDeviceSize cameraSize, VkBuffer modelBuffer,
                           VkDeviceSize modelSize) {
  VkDescriptorBufferInfo bufferInfos[2]{};
  bufferInfos[0].buffer = cameraBuffer;
  bufferInfos[0].offset = frameIndex * cameraSize;
  bufferInfos[0].range = cameraSize;

  bufferInfos[1].buffer = modelBuffer;
  bufferInfos[1].offset = 0;
  bufferInfos[1].range = modelSize;

  VkWriteDescriptorSet descriptorWrites[2]{};

  // Binding 0: Camera UBO
  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].dstSet = descriptorSets[frameIndex];
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pBufferInfo = &bufferInfos[0];

  // Binding 1: Model UBO
  descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[1].dstSet = descriptorSets[frameIndex];
  descriptorWrites[1].dstBinding = 1;
  descriptorWrites[1].dstArrayElement = 0;
  descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[1].descriptorCount = 1;
  descriptorWrites[1].pBufferInfo = &bufferInfos[1];

  vkUpdateDescriptorSets(device.getLogical(), 2, descriptorWrites, 0, nullptr);
}

VkDescriptorSet &DescriptorSet::get(uint32_t frameIndex) {
  return descriptorSets[frameIndex];
}
