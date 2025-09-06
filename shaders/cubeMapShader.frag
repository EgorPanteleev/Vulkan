#version 460

layout(location = 0) out vec4 outColor;

void main() {
    vec3 result = vec3(1, 0, 0);
    outColor = vec4(result, 1);
}