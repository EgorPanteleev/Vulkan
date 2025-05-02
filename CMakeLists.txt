cmake_minimum_required(VERSION 3.31)
project(Vulkan)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "-O2")

add_subdirectory(src)
add_subdirectory(thirdparty)
add_subdirectory(shaders)