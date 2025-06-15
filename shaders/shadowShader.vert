#version 460
layout(location = 0) in vec3 inPosition;

layout(binding = 0) uniform DirectLight {
    mat4 VPMatrix;
    vec4 color;
    vec4 direction;
} directLight;

layout(binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = directLight.VPMatrix * ubo.model * vec4(inPosition, 1.0);
}