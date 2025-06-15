#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 2) uniform DirectLight {
    mat4 VPMatrix;
    vec4 color;
    vec4 direction;
} directLight;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in uint inTexIndex;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPosition;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out flat uint fragTexIndex;
layout(location = 5) out vec4 fragPosLightSpace;


void main() {
    vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPosition;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragPosition = worldPosition.xyz;
    fragNormal = mat3(ubo.model) * inNormal;
    fragPosLightSpace = directLight.VPMatrix * worldPosition;
    fragTexIndex = inTexIndex;
}