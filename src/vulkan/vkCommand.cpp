#include "vkCommand.h"
#include "vkDevice.h"
#include <stdexcept>

vCommand::vCommand(VkPhysicalDevice physicalDevice, VkDevice device,
                   VkSurfaceKHR surface)
    : device(device) {
  QueueFamilyIndices queueFamilyIndices =
      vDevice::findQueueFamilies(physicalDevice, surface);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void vCommand::createCommandBuffers(VkDevice device,
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

void vCommand::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                   uint32_t imageIndex, VkRenderPass renderPass,
                                   vSwapchain &swapchain, vPipeline &pipeline,
                                   VkBuffer vertexBuffer) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = swapchain.getSwapchainFramebuffers()[imageIndex];

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapchain.getSwapchainExtent();

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.getGraphicsPipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapchain.getSwapchainExtent().width);
  viewport.height = static_cast<float>(swapchain.getSwapchainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapchain.getSwapchainExtent();
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

std::vector<VkCommandBuffer> &vCommand::getCommandBuffers() noexcept {
  return commandBuffers;
}

vCommand::~vCommand() { vkDestroyCommandPool(device, commandPool, nullptr); }

VkCommandPool vCommand::getCommandPool() const noexcept { return commandPool; }
