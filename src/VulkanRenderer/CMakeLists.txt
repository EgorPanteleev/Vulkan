project(VulkanRenderer)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

#static Library
file(GLOB SOURCES "./private/*.cpp" "./private/UniformBuffers/*.cpp" "./private/Image/*.cpp")
file(GLOB HEADERS "./public/*.hpp"  "./public/UniformBuffers/*.hpp"  "./public/Image/*.hpp")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ./public/ ./public/UniformBuffers/ ./public/Image/)

set(Libs ModelLoader Utils Camera Message)
set(ThirdPartyLibs vma stbImage gli imgui Tracy::TracyClient)
target_link_libraries(${PROJECT_NAME} ${Libs} ${ThirdPartyLibs})