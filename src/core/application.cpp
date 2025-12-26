#include "core/application.h"
#include "renderer/renderer.h"
#include "renderer/uniforms.h"
#include <vulkan/vulkan_core.h>

Application::Application()
    : instance(enableValidationLayers), window("vkPrac", 800, 600),
      surface(instance.getInstance(), window),
      device(instance, surface.get(), enableValidationLayers),
      swapchain(device, surface.get(), window),
      pipeline(device.getLogical(), swapchain.getSwapchainImageFormat()),
      commandContext(device.getPhysical(), device.getLogical(), surface.get()),
      recorder(pipeline), frame(device, swapchain.getSwapchain()),
      renderer(device, swapchain, commandContext, recorder, frame) {
  initVulkan();
}
void Application::initVulkan() {
  commandContext.allocate(frame.getMaxFramesInFlight());

  // --- Camera FIRST ---
  camera = std::make_unique<Camera>(device, commandContext.getPool(),
                                    frame.getMaxFramesInFlight());

  camera->setPerspective(45.0f, window.getAspectRatio(), 0.1f, 100.0f);
  camera->setPosition({1.0f, 1.0f, 5.0f});
  camera->lookAt({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

  // --- Mesh ---
  auto mesh = std::make_unique<Mesh>(device, commandContext.getPool());

  VkDeviceSize vbSize = sizeof(Vertex) * pipeline.vertices.size();
  VkDeviceSize ibSize = sizeof(uint32_t) * pipeline.indices.size();

  mesh->vertexBuffer.create(vbSize,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  mesh->vertexBuffer.uploadViaStaging(pipeline.vertices.data(), vbSize);

  mesh->indexBuffer.create(ibSize,
                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  mesh->indexBuffer.uploadViaStaging(pipeline.indices.data(), ibSize);

  mesh->indexCount = static_cast<uint32_t>(pipeline.indices.size());
  meshes.push_back(std::move(mesh));

  // --- RenderItem ---
  auto item = std::make_unique<RenderItem>(device, commandContext.getPool());
  item->mesh = meshes.back().get();
  item->transform = glm::mat4(1.0f);

  item->modelBuffer.create(sizeof(ModelUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  item->descriptorSet = std::make_unique<DescriptorSet>(
      device, pipeline.getDescriptorSetLayout(), frame.getMaxFramesInFlight());

  for (uint32_t f = 0; f < frame.getMaxFramesInFlight(); f++) {
    item->descriptorSet->update(f, camera->getBuffer(), sizeof(CameraUBO),
                                item->modelBuffer.get(), sizeof(ModelUBO));
  }

  renderItems.push_back(std::move(item));
}
void Application::mainLoop() {
  while (!window.shouldClose()) {
    window.pollEvents();
    std::vector<RenderItem *> rawPtrs;
    rawPtrs.reserve(renderItems.size());
    for (auto &r : renderItems)
      rawPtrs.push_back(r.get());
    RenderResult result = renderer.drawFrame(rawPtrs, *camera);

    if (result == RenderResult::SwapchainOutOfDate ||
        window.getFrameBufferResized()) {

      window.setFrameBufferResized(false);
      swapchain.recreateSwapchain(surface.get(), window);
    } else if (result == RenderResult::FatalError) {
      throw std::runtime_error("Fatal render error");
    }
  }
  vkDeviceWaitIdle(device.getLogical());
}

Application::~Application() {}
