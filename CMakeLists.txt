cmake_minimum_required(VERSION 3.20)
project(soulslike LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
add_executable(${PROJECT_NAME} src/core/main.cpp)

file(GLOB_RECURSE RENDERER_SRC "src/renderer/*.cpp")
file(GLOB_RECURSE RHI_VK_SRC "src/rhi/vulkan/*.cpp")
file(GLOB_RECURSE CORE_SRC "src/core/*.cpp")
file(GLOB_RECURSE GAME_SRC "src/game/*.cpp")
file(GLOB_RECURSE SCENE_SRC "src/scene/*.cpp")

target_sources(${PROJECT_NAME} PRIVATE ${RENDERER_SRC} ${RHI_VK_SRC}
                                       ${CORE_SRC} ${GAME_SRC} ${SCENE_SRC})

# target_sources( ${PROJECT_NAME} PRIVATE src/vulkan/vk_device.cpp
# src/vulkan/vk_instance.cpp src/vulkan/vk_surface.cpp
# src/vulkan/vk_swapchain.cpp src/vulkan/vk_frame.cpp
# src/vulkan/vk_framebuffers.cpp src/vulkan/vk_renderpass.cpp
# src/vulkan/vk_buffer.cpp)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan glfw)
