#include "application.h"
#include "vulkan/vkRenderPass.h"
#include <vulkan/vulkan_core.h>

Application::Application()
    : instance(enableValidationLayers), window("vkPrac", 800, 600),
      surface(instance.getInstance(), window),
      device(instance, surface.get(), enableValidationLayers),
      swapchain(device, surface.get(), window),
      renderPass(swapchain.getSwapchainImageFormat(), device.getLogical()),
      pipeline(device.getLogical(), renderPass.get()),
      command(device.getPhysical(), device.getLogical(), surface.get()),
      frame(device), vertexBuffer(device.getLogical(), device.getPhysical()) {
  initVulkan();
}
void Application::initVulkan() {
  swapchain.createImageViews();
  swapchain.createFramebuffers(renderPass.get());
  command.createCommandBuffers(device.getLogical(),
                               frame.getMaxFramesInFlight());
}
void Application::mainLoop() {
  while (!window.shouldClose()) {
    window.pollEvents();
    frame.drawFrame(currentFrame, swapchain, surface.get(), renderPass.get(),
                    window, vertexBuffer.get(), command, pipeline);
  }
  vkDeviceWaitIdle(device.getLogical());
}

Application::~Application() { swapchain.cleanupSwapChain(); }
