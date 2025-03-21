cmake_minimum_required(VERSION 3.31)
project(Vulkan)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "-O2")

#Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(VulkanApp main.cpp)


set(Libs -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
#static Library
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries( VulkanApp ${PROJECT_NAME} ${Libs} )

#add_subdirectory( Shaders )
