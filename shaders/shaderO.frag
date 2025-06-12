#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec4 fragPosLightSpace;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

//layout(binding = 3) uniform sampler2D shadowMap;

const int MAX_LIGHTS = 10;

layout(std140, binding = 2) uniform Light {
    vec4 lightPos[MAX_LIGHTS];
    vec4 lightColor[MAX_LIGHTS];
    vec4 viewPos;
    int lightCount;
    int padding[3];
} lights;

layout(binding = 3) uniform sampler2D shadowMap;
layout(binding = 4) uniform UniformDirectLight {
    mat4 view;
    mat4 proj;
    vec4 color;
    vec4 direction;
} directLight;

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

void main() {
    // fragPosLightSpace - координаты фрагмента в пространстве света, передаёшь из вершинного шейдера
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // преобразуем из [-1;1] в [0;1]

    float depth = texture(shadowMap, projCoords.xy).r;
    outColor = vec4(vec3(depth), 1.0);


        vec3 viewDir = normalize(lights.viewPos.xyz - fragPos);
        vec3 result = vec3(0.0);

        vec3 normal = normalize(fragNormal);
        vec3 lightDir = normalize(-directLight.direction.xyz);
        vec3 diffuseColor = max(dot(normal, lightDir), 0.0) * directLight.color.xyz;

        //outColor = vec4(diffuseColor, 1) * ( 1.0 - shadow );
        outColor = vec4(diffuseColor, 1);
}