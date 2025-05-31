project(General)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(ThirdPartyLibs vma)
set(Libs Message Geometry)

#static Library
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} ${ThirdPartyLibs} ${Libs})
