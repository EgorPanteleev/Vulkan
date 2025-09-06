project(VulkanRenderer)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

#static Library
file(GLOB_RECURSE SOURCES "./private/*.cpp")
file(GLOB_RECURSE HEADERS "./public/*.hpp")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

set(INCLUDE_DIRS
        ./public
        ./public/UniformBuffers/
        ./public/Image/
        ./public/Pipelines/ )
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

set(Libs ModelLoader Utils Camera Message)
set(ThirdPartyLibs vma stbImage gli imgui Tracy::TracyClient)
target_link_libraries(${PROJECT_NAME} ${Libs} ${ThirdPartyLibs})