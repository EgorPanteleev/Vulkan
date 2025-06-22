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
layout(location = 4) in vec4 inTangent;
layout(location = 5) in uint inTexIndex;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPosition;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragTangent;
layout(location = 5) out vec3 fragBitangent;
layout(location = 6) out flat uint fragAlbedoIndex;
layout(location = 7) out flat uint fragNormalIndex;
layout(location = 8) out vec4 fragPosLightSpace;


void main() {
    mat3 normalMatrix = transpose(inverse(mat3(ubo.model)));
    vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPosition;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragPosition = worldPosition.xyz;
    fragNormal = normalize(normalMatrix * inNormal);
    fragTangent = normalize(normalMatrix * inTangent.xyz);
    fragBitangent = cross(fragNormal, fragTangent) * inTangent.w;
    fragPosLightSpace = directLight.VPMatrix * worldPosition;
    fragAlbedoIndex = inTexIndex;
    fragNormalIndex = inTexIndex + 1;
}