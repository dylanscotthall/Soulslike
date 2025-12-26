#pragma once
#include <vector>
#include <vulkan/vulkan.h>

class CommandContext {
public:
  CommandContext(VkPhysicalDevice phys, VkDevice device, VkSurfaceKHR surface);

  ~CommandContext();

  void allocate(uint32_t framesInFlight);

  VkCommandBuffer get(uint32_t frame) const noexcept;
  VkCommandPool getPool() const noexcept;

private:
  VkDevice device{};
  VkCommandPool pool{};
  std::vector<VkCommandBuffer> buffers;
};
