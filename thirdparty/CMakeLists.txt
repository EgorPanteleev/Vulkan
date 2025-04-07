cmake_minimum_required( VERSION 3.31 )

add_library(stbImage INTERFACE)
target_include_directories(stbImage INTERFACE ./stbi/)
