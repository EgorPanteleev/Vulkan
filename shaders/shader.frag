#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 normal;
//layout(location = 4) in vec4 fragPosLightSpace; // Position in light space

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 0) uniform sampler2DShadow shadowMap;

const int MAX_LIGHTS = 10;

layout(std140, binding = 2) uniform Light {
    vec4 lightPos[MAX_LIGHTS];
    vec4 lightColor[MAX_LIGHTS];
    vec4 viewPos;
    int lightCount;
    int padding[3];
} lights;

float calculateShadow(vec4 fragPosLightSpace) {
    // Transform to [0, 1] range
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Check if the fragment is outside the light's frustum
    if (projCoords.z > 1.0) {
        return 1.0;
    }

    // Perform depth comparison
    float shadow = texture(shadowMap, projCoords);
    return shadow;
}

vec3 calculateBlinnPhong(vec3 normal, vec3 fragPos, vec3 viewDir, vec3 lightPos, vec3 lightColor, float shadowFactor) {
    // Ambient lighting
    //return vec3(shadowFactor);
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

    // Apply shadow factor
    diffuse *= shadowFactor;
    specular *= shadowFactor;

    return ambient + diffuse + specular;
}

const mat4 lightViewProj = mat4(
// Orthographic projection (left, right, bottom, top, near, far)
0.1, 0.0, 0.0, 0.0,  // scale X
0.0, 0.1, 0.0, 0.0,  // scale Y
0.0, 0.0, 0.02, 0.0, // scale Z
0.01, 0.01, 0.01, 1  // translation (move to positive coords)
);

void main() {
    vec3 viewDir = normalize(lights.viewPos.xyz - fragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < lights.lightCount; ++i) {
        // Calculate fragment position in light space (should be passed as input or computed)
        vec4 fragPosLightSpace = lightViewProj * vec4(fragPos, 1.0);

        // Calculate shadow factor based on fragment's position in light space
        float shadowFactor = calculateShadow(fragPosLightSpace);; //calculateShadow(fragPosLightSpace);

        // Calculate lighting modulated by shadow factor
        result += calculateBlinnPhong(normal, fragPos, viewDir, lights.lightPos[i].xyz, lights.lightColor[i].xyz, 1.0);

        // Reduce light contribution by shadow factor (simple shadowing)
    }

    // Sample texture color
    vec4 texColor = texture(texSampler, fragTexCoord);

    // Combine lighting result with vertex color and texture color
    vec3 finalColor = result * fragColor;

    outColor = vec4(finalColor, 1);
}