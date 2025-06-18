add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}/")
add_compile_definitions(BINARY_PATH="${CMAKE_BINARY_DIR}/")
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
#add_compile_definitions(GLM_FORCE_RIGHT_HANDED)
add_compile_definitions(GLM_FORCE_RADIANS)

add_subdirectory(Camera)
add_subdirectory(Message)
add_subdirectory(Geometry)
add_subdirectory(ModelLoader)

add_subdirectory(General)
add_subdirectory(UniformBuffers)
add_subdirectory(ShadowPipeline)
add_subdirectory(GraphicsPipeline)

set(Libs Camera General GraphicsPipeline ShadowPipeline)

#static Library
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
add_library(VulkanLib STATIC ${SOURCES} ${HEADERS})

target_link_libraries(VulkanLib ${Libs} )
target_include_directories(VulkanLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Applications)