#include "command.h"
#include "device.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Command::Command(VkPhysicalDevice physicalDevice, VkDevice device,
                 VkSurfaceKHR surface, Pipeline &pipeline)
    : device(device), pipeline(pipeline) {
  QueueFamilyIndices queueFamilyIndices =
      Device::findQueueFamilies(physicalDevice, surface);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void Command::createCommandBuffers(VkDevice device,
                                   const int MAX_FRAMES_IN_FLIGHT) {
  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void Command::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                  uint32_t imageIndex, Swapchain &swapchain,
                                  std::span<const RenderItem> items) {

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("vkBeginCommandBuffer failed");
  }

  // --- Transition: UNDEFINED -> COLOR ATTACHMENT_OPTIMAL ---
  VkImageMemoryBarrier2 toAttachment{};
  toAttachment.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  toAttachment.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  toAttachment.srcAccessMask = 0;
  toAttachment.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  toAttachment.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  toAttachment.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  toAttachment.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  toAttachment.image = swapchain.getSwapchainImages()[imageIndex];
  toAttachment.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  toAttachment.subresourceRange.levelCount = 1;
  toAttachment.subresourceRange.layerCount = 1;

  // --- NEW: Transition DEPTH IMAGE UNDEFINED -> DEPTH_ATTACHMENT_OPTIMAL ---
  VkImageMemoryBarrier2 depthBarrier{};
  depthBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  depthBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  depthBarrier.srcAccessMask = 0;
  depthBarrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                              VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
  depthBarrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  depthBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
  depthBarrier.image = swapchain.getDepthImage(); // **image, not imageView**
  depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  depthBarrier.subresourceRange.baseMipLevel = 0;
  depthBarrier.subresourceRange.levelCount = 1;
  depthBarrier.subresourceRange.baseArrayLayer = 0;
  depthBarrier.subresourceRange.layerCount = 1;

  VkImageMemoryBarrier2 barriers[] = {toAttachment, depthBarrier};
  VkDependencyInfo depInfo{};
  depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  depInfo.imageMemoryBarrierCount = 2;
  depInfo.pImageMemoryBarriers = barriers;

  vkCmdPipelineBarrier2(commandBuffer, &depInfo);

  // --- Dynamic Rendering ---
  VkRenderingAttachmentInfo colorAttachment{};
  colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  colorAttachment.imageView = swapchain.getSwapchainImageViews()[imageIndex];
  colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.clearValue = {{{1.f, 0.f, 0.f, 1.f}}};

  VkRenderingAttachmentInfo depthAttachment{};
  depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  depthAttachment.imageView = swapchain.getDepthImageView(); // view here
  depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.clearValue.depthStencil = {1.0f, 0};

  VkRenderingInfo renderingInfo{};
  renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  renderingInfo.renderArea = {{0, 0}, swapchain.getSwapchainExtent()};
  renderingInfo.layerCount = 1;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments = &colorAttachment;
  renderingInfo.pDepthAttachment = &depthAttachment;

  vkCmdBeginRendering(commandBuffer, &renderingInfo);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.getGraphicsPipeline());

  VkViewport viewport{};
  viewport.width = static_cast<float>(swapchain.getSwapchainExtent().width);
  viewport.height = static_cast<float>(swapchain.getSwapchainExtent().height);
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.extent = swapchain.getSwapchainExtent();
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  for (const RenderItem &item : items) {
    const Mesh &mesh = *item.mesh;

    VkBuffer vb = mesh.vertexBuffer.get();
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vb, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer.get(), 0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
  }

  vkCmdEndRendering(commandBuffer);

  // --- Transition: COLOR ATTACHMENT_OPTIMAL -> PRESENT ---
  VkImageMemoryBarrier2 toPresent{};
  toPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  toPresent.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  toPresent.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  toPresent.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
  toPresent.dstAccessMask = 0;
  toPresent.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  toPresent.image = swapchain.getSwapchainImages()[imageIndex];
  toPresent.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  toPresent.subresourceRange.levelCount = 1;
  toPresent.subresourceRange.layerCount = 1;

  VkDependencyInfo depInfo2{};
  depInfo2.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  depInfo2.imageMemoryBarrierCount = 1;
  depInfo2.pImageMemoryBarriers = &toPresent;

  vkCmdPipelineBarrier2(commandBuffer, &depInfo2);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("vkEndCommandBuffer failed");
  }
}
std::vector<VkCommandBuffer> &Command::getCommandBuffers() noexcept {
  return commandBuffers;
}

Command::~Command() { vkDestroyCommandPool(device, commandPool, nullptr); }

VkCommandPool Command::getCommandPool() const noexcept { return commandPool; }
