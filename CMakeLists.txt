cmake_minimum_required(VERSION 3.20)
project(vkPrac LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug symbols
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Optional: more explicit debug flags set(CMAKE_CXX_FLAGS_DEBUG
# "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g")

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp)

target_sources(
  ${PROJECT_NAME}
  PRIVATE src/application.cpp
          src/vulkan/window.cpp
          src/vulkan/vkInstance.cpp
          src/vulkan/surface.cpp
          src/vulkan/vkDevice.cpp
          src/vulkan/vkSwapchain.cpp
          src/vulkan/vkRenderPass.cpp
          src/vulkan/vkPipeline.cpp
          src/vulkan/vkCommand.cpp
          src/vulkan/vkFrame.cpp
          src/vulkan/vkBuffer.cpp)

# target_sources( ${PROJECT_NAME} PRIVATE src/vulkan/vk_device.cpp
# src/vulkan/vk_instance.cpp src/vulkan/vk_surface.cpp
# src/vulkan/vk_swapchain.cpp src/vulkan/vk_frame.cpp
# src/vulkan/vk_framebuffers.cpp src/vulkan/vk_renderpass.cpp
# src/vulkan/vk_buffer.cpp)

# Include directories target_include_directories(${PROJECT_NAME} PRIVATE include
# shaders src)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan glfw)
