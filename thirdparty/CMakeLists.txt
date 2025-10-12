#VulkanMemoryAllocator
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE ./VulkanMemoryAllocator/include/)

#gli
add_subdirectory(./gli)

#Tracy
option (TRACY_ENABLE "" ON)
option (TRACY_ON_DEMAND "" ON)
add_subdirectory(./tracy)

add_subdirectory(./Collection)
