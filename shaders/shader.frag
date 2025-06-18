#version 460

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_descriptor_indexing : enable

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

vec3 calculateBlinnPhong(vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 lightColor) {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting (Blinn-Phong)
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    return ambient + diffuse + specular;
}

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float bias = max(0.001, 0.005 * (1.0 - dot(normal, lightDir)));
//    float bias = 0.005;
    float currentDepth = projCoords.z - bias;
    return currentDepth > closestDepth ? 1.0 : 0.0;
}

void main() {
    vec4 texColor = texture(nonuniformEXT(textures[fragTexIndex]), fragTexCoord);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    if (any(lessThan(projCoords, vec3(0.0))) || any(greaterThan(projCoords, vec3(1.0)))) {
        outColor = vec4(1, 0, 0, 1);
        return;
    }

    vec3 lightDir = normalize(-directLight.direction.xyz);
    vec3 normal = normalize(fragNormal);

    float depth = texture(shadowMap, projCoords.xy).r;
    float shadow = calculateShadow(fragPosLightSpace, normal, lightDir);

    vec3 viewDir = normalize(pointLights.viewPos.xyz - fragPos);
    vec4 diffuseColor = vec4(max(dot(normal, lightDir), 0.0) * directLight.color.xyz, 1);
    //outColor = texColor;
    //outColor = texColor * diffuseColor;
    //outColor = vec4(texColor.xyz * (1 - shadow), 1);
    //outColor = vec4(vec3(depth), 1.0);
    outColor = vec4(texColor.xyz * diffuseColor.xyz * (1 - shadow), 1);
}