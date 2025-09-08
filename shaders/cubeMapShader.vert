#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 v_dir;

void main() {
    mat4 viewNoTrans = mat4(mat3(ubo.view));
    gl_Position = ubo.proj * viewNoTrans * vec4(inPos, 1.0);
    v_dir = inPos;
}