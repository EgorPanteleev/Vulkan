#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 dir;

void main() {
    mat4 rotView = mat4(mat3(ubo.view));
    dir = (rotView * vec4(inPos, 0.0)).xyz;
    gl_Position = ubo.proj * rotView * vec4(inPos, 1.0);
}