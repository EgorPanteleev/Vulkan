set(ThirdPartyLibs glfw Vulkan::Vulkan)
set(Libs VulkanLib)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(VulkanApp FirstApp.cpp)

target_link_libraries(VulkanApp ${ThirdPartyLibs} ${Libs})
