#version 450
layout(location = 0) in vec3 inPosition;

//layout(binding = 0) uniform sampler2D shadowSampler;

layout(binding = 1) uniform UniformDirectLight {
    mat4 view;
    mat4 proj;
    vec4 color;
    vec4 direction;
} directLight;

layout(binding = 2) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = directLight.proj * directLight.view * ubo.model * vec4(inPosition, 1.0);
}