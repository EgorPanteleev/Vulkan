#Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(Message)

add_subdirectory(Geometry)

add_subdirectory(ModelLoader)

add_executable(VulkanApp main.cpp)

set(Libs -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
#static Library
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
add_library(VulkanLib STATIC ${SOURCES} ${HEADERS})

target_link_libraries(VulkanLib stbImage vma Geometry ModelLoader Message)

target_link_libraries(VulkanApp VulkanLib ${Libs})
