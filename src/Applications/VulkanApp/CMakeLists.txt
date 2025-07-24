set(ThirdPartyLibs glfw Vulkan::Vulkan Tracy::TracyClient)
set(Libs VulkanLib)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB SOURCES "*.cpp")
add_executable(VulkanApp ${SOURCES})
target_compile_definitions(VulkanApp PRIVATE TRACY_ENABLE)

target_link_libraries(VulkanApp ${ThirdPartyLibs} ${Libs})
