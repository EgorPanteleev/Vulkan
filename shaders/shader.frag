#version 460

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) flat in uint fragTexIndex;
layout(location = 5) in vec4 fragPosLightSpace;

layout(binding = 1) uniform sampler2D shadowMap;

layout(binding = 2) uniform DirectLight {
    mat4 VPMatrix;
    vec4 color;
    vec4 direction;
} directLight;

const int MAX_LIGHTS = 10;
layout(std140, binding = 3) uniform PointLight {
    vec4 lightPos[MAX_LIGHTS];
    vec4 lightColor[MAX_LIGHTS];
    vec4 viewPos;
    int lightCount;
    int padding[3];
} pointLights;

layout(binding = 4) uniform sampler2D textures[];

layout(location = 0) out vec4 outColor;

const int PCF_SAMPLES = 3;
const float BIAS = 0.005;
float calculateShadowPCF(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float currentDepth = projCoords.z - BIAS;
    float shadow = 0;
    for (int x = -PCF_SAMPLES / 2; x <= PCF_SAMPLES / 2; ++x) {
        for (int y = -PCF_SAMPLES / 2; y <= PCF_SAMPLES / 2; ++y) {
            vec2 offset = texelSize * vec2(x, y);
            float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
            shadow += currentDepth > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow / (PCF_SAMPLES * PCF_SAMPLES);
}

void main() {
    vec4 texColor = texture(nonuniformEXT(textures[fragTexIndex]), fragTexCoord);

    vec3 lightDir = normalize(-directLight.direction.xyz);
    vec3 normal = normalize(fragNormal);

    float shadow = calculateShadowPCF(fragPosLightSpace);

    vec3 viewDir = normalize(pointLights.viewPos.xyz - fragPos);
    // Diffuse
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * directLight.color.xyz;
    // Ambient
    float ambientStrength = 0.03f;
    vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f); //(0.8, 0.9, 1.0)
    vec3 ambient = ambientStrength * ambientColor;
    // Specular (Blinn-Phong)
    float shininess = 4;
    float specularStrength = 0.2;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * directLight.color.xyz;

    vec3 result = (ambient + (diffuse + specular) * (1 - shadow))* texColor.xyz;
    outColor = vec4(result, 1);

    //outColor = texColor;
    //outColor = texColor * diffuseColor;
    //outColor = vec4(texColor.xyz * (1 - shadow), 1);
    //outColor = vec4(texColor.xyz * diffuseColor.xyz * (1 - shadow), 1);
}