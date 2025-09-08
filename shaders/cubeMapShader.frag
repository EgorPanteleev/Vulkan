#version 460

layout(set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 v_dir;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 dir = normalize(v_dir);
    outColor = texture(skybox, dir);
}