project(Application)

set(ThirdPartyLibs glfw Vulkan::Vulkan Tracy::TracyClient)
set(Libs VulkanRenderer)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB SOURCES "./private/*.cpp")
add_executable(VulkanApp ${SOURCES})
target_compile_definitions(VulkanApp PRIVATE TRACY_ENABLE)
target_include_directories(VulkanApp PUBLIC ./public/)

target_link_libraries(VulkanApp ${ThirdPartyLibs} ${Libs})