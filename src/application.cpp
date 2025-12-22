#include "application.h"
#include <vulkan/vulkan_core.h>

Application::Application()
    : instance(enableValidationLayers), window("vkPrac", 800, 600),
      surface(instance.getInstance(), window),
      device(instance, surface.get(), enableValidationLayers),
      swapchain(device, surface.get(), window),
      pipeline(device.getLogical(), swapchain.getSwapchainImageFormat()),
      command(device.getPhysical(), device.getLogical(), surface.get()),
      frame(device, swapchain.getSwapchain()),
      vertexBuffer(device, command.getCommandPool()),
      indexBuffer(device, command.getCommandPool()) {
  initVulkan();
}
void Application::initVulkan() {
  swapchain.createImageViews();
  command.createCommandBuffers(device.getLogical(),
                               frame.getMaxFramesInFlight());
  VkDeviceSize vertexBufferSize = sizeof(Vertex) * pipeline.vertices.size();
  VkDeviceSize indexBufferSize = sizeof(uint32_t) * pipeline.indices.size();
  vertexBuffer.create(vertexBufferSize,
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vertexBuffer.uploadViaStaging(pipeline.vertices.data(), vertexBufferSize);
  indexBuffer.create(indexBufferSize,
                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  indexBuffer.uploadViaStaging(pipeline.indices.data(), indexBufferSize);
}
void Application::mainLoop() {
  while (!window.shouldClose()) {
    window.pollEvents();
    frame.drawFrame(currentFrame, swapchain, surface.get(), window,
                    vertexBuffer.get(), indexBuffer.get(), command, pipeline);
  }
  vkDeviceWaitIdle(device.getLogical());
}

Application::~Application() { swapchain.cleanupSwapChain(); }
