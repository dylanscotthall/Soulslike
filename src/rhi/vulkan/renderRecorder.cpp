#include "rhi/vulkan/renderRecorder.h"
#include "helper.h"

RenderRecorder::RenderRecorder(Pipeline &pipeline) : pipeline(pipeline) {}

void RenderRecorder::record(VkCommandBuffer cmd, Swapchain &swapchain,
                            uint32_t imageIndex, uint32_t frame,
                            std::span<RenderItem *> items, Camera &camera) {
  VkCommandBufferBeginInfo begin{};
  begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VK_CHECK(vkBeginCommandBuffer(cmd, &begin));

  // --- Image barriers for color and depth ---
  VkImageMemoryBarrier2 barriers[2]{};

  // Color
  barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  barriers[0].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  barriers[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barriers[0].newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  barriers[0].image = swapchain.getSwapchainImages()[imageIndex];
  barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barriers[0].subresourceRange.levelCount = 1;
  barriers[0].subresourceRange.layerCount = 1;

  // Depth
  barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  barriers[1].dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
  barriers[1].dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barriers[1].newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
  barriers[1].image = swapchain.getDepthImage();
  barriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  barriers[1].subresourceRange.levelCount = 1;
  barriers[1].subresourceRange.layerCount = 1;

  VkDependencyInfo dep{};
  dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dep.imageMemoryBarrierCount = 2;
  dep.pImageMemoryBarriers = barriers;

  vkCmdPipelineBarrier2(cmd, &dep);

  // --- Dynamic rendering setup ---
  VkRenderingAttachmentInfo colorAtt{};
  colorAtt.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  colorAtt.imageView = swapchain.getSwapchainImageViews()[imageIndex];
  colorAtt.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  colorAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAtt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAtt.clearValue.color = {{0.01f, 0.01f, 0.01f, 1.f}};

  VkRenderingAttachmentInfo depthAtt{};
  depthAtt.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  depthAtt.imageView = swapchain.getDepthImageView();
  depthAtt.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
  depthAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAtt.clearValue.depthStencil = {1.f, 0};

  VkRenderingInfo ri{};
  ri.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  ri.renderArea.extent = swapchain.getSwapchainExtent();
  ri.layerCount = 1;
  ri.colorAttachmentCount = 1;
  ri.pColorAttachments = &colorAtt;
  ri.pDepthAttachment = &depthAtt;

  vkCmdBeginRendering(cmd, &ri);

  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.getGraphicsPipeline());

  VkViewport vp{};
  vp.width = (float)swapchain.getSwapchainExtent().width;
  vp.height = (float)swapchain.getSwapchainExtent().height;
  vp.maxDepth = 1.f;
  vkCmdSetViewport(cmd, 0, 1, &vp);

  VkRect2D sc{};
  sc.extent = swapchain.getSwapchainExtent();
  vkCmdSetScissor(cmd, 0, 1, &sc);

  // --- Draw items ---
  for (auto *item : items) {
    // Update per-frame UBOs
    item->update(frame, camera.getBuffer(), sizeof(CameraUBO));

    auto &mesh = *item->mesh;
    VkDeviceSize offset = 0;
    VkBuffer vb = mesh.vertexBuffer.get();

    vkCmdBindVertexBuffers(cmd, 0, 1, &vb, &offset);
    vkCmdBindIndexBuffer(cmd, mesh.indexBuffer.get(), 0, VK_INDEX_TYPE_UINT32);

    // Single descriptor set with both camera and model bindings
    VkDescriptorSet sets[] = {item->descriptorSet->get(frame)};
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline.getPipelineLayout(), 0, 1, sets, 0,
                            nullptr);

    vkCmdDrawIndexed(cmd, mesh.indexCount, 1, 0, 0, 0);
  }

  vkCmdEndRendering(cmd);

  // --- Transition color image to present ---
  VkImageMemoryBarrier2 present{};
  present.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  present.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  present.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  present.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  present.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  present.image = swapchain.getSwapchainImages()[imageIndex];
  present.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  present.subresourceRange.levelCount = 1;
  present.subresourceRange.layerCount = 1;

  VkDependencyInfo dep2{};
  dep2.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dep2.imageMemoryBarrierCount = 1;
  dep2.pImageMemoryBarriers = &present;

  vkCmdPipelineBarrier2(cmd, &dep2);

  VK_CHECK(vkEndCommandBuffer(cmd));
}
