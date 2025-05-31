# Define the input directory of shaders
set(SRC_SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")

# Define the shader compilation script
set(SHADER_COMPILER_SCRIPT "${SRC_SHADER_DIR}/compile.sh")

# Define the output directory for compiled shaders
set(COMPILED_SHADER_DIR "${CMAKE_BINARY_DIR}/compiled_shaders")

add_custom_target(
        CompileShaders
        COMMAND ${CMAKE_COMMAND} -E make_directory "${COMPILED_SHADER_DIR}"
        COMMAND ${SHADER_COMPILER_SCRIPT} "${SRC_SHADER_DIR}" "${COMPILED_SHADER_DIR}" > ${COMPILED_SHADER_DIR}/shader_compile.log 2>&1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling shaders..."
)

# Ensure the custom target runs before the main build
add_dependencies(VulkanApp CompileShaders)