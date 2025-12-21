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
      vertexBuffer(device.getLogical(), device.getPhysical(),
                   pipeline.vertices) {
  initVulkan();
}
void Application::initVulkan() {
  swapchain.createImageViews();
  command.createCommandBuffers(device.getLogical(),
                               frame.getMaxFramesInFlight());
}
void Application::mainLoop() {
  while (!window.shouldClose()) {
    window.pollEvents();
    frame.drawFrame(currentFrame, swapchain, surface.get(), window,
                    vertexBuffer.get(), command, pipeline);
  }
  vkDeviceWaitIdle(device.getLogical());
}

Application::~Application() { swapchain.cleanupSwapChain(); }
