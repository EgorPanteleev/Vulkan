project(ModelLoader)

find_package(assimp REQUIRED)

#static Library
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(Libs Geometry Message)
set(ThirdPartyLibs assimp::assimp tinyobjloader)
target_link_libraries(${PROJECT_NAME} ${Libs} ${ThirdPartyLibs})