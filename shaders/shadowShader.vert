#version 450
layout(location = 0) in vec3 inPos;

//layout(set = 0, binding = 0) uniform LightVP {
//    mat4 lightViewProj;
//} ubo;

void main() {
    gl_Position = vec4(inPos, 1.0);
}