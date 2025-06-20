cmake_minimum_required( VERSION 3.31 )

add_library(stbImage INTERFACE)
target_include_directories(stbImage INTERFACE ./stbi/)

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE ./VulkanMemoryAllocator/include/)

add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE ./tinyobjloader/ )

#static Library
file(GLOB IMGUI_SOURCES "./imgui/*.cpp" "./imgui/backends/*vulkan.cpp" "./imgui/backends/*glfw.cpp")
file(GLOB IMGUI_HEADERS "./imgui/*.h" "./imgui/backends/*vulkan.h" "./imgui/backends/*glfw.h")
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC ./imgui/)

add_subdirectory(./meshoptimizer)
