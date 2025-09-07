#version 460

layout(location = 0) in vec3 v_dir;
layout(location = 0) out vec4 outColor;

void main() {
//    vec3 result = vec3(1, 0, 0);
//    outColor = vec4(result, 1);
    vec3 dir = normalize(v_dir);
    vec3 ad = abs(dir);

    // Determine which face is major (X, Y or Z)
    float maxComp = max(max(ad.x, ad.y), ad.z);

    vec3 color;

    // Compute a simple per-face UV so we get a subtle gradient on each face
    // For each face, compute two coordinates (u,v) from dir components
    vec2 uv;
    if (maxComp == ad.x) {
        // +X or -X face
        uv = vec2(dir.z, dir.y) / ad.x;
        if (dir.x > 0.0) {
            // +X face: RED-ish
            color = vec3(1.0, 0.15 + 0.35 * uv.y, 0.15 + 0.35 * uv.x);
        } else {
            // -X face: GREEN-ish
            color = vec3(0.15 + 0.35 * uv.x, 1.0, 0.15 + 0.35 * uv.y);
        }
    } else if (maxComp == ad.y) {
        // +Y or -Y face
        uv = vec2(dir.x, dir.z) / ad.y;
        if (dir.y > 0.0) {
            // +Y face: BLUE-ish
            color = vec3(0.15 + 0.35 * uv.x, 0.15 + 0.35 * uv.y, 1.0);
        } else {
            // -Y face: YELLOW-ish (red + green)
            color = vec3(1.0, 0.95 - 0.25 * uv.x, 0.15 + 0.25 * uv.y);
        }
    } else {
        // Z is major
        uv = vec2(dir.x, dir.y) / ad.z;
        if (dir.z > 0.0) {
            // +Z face: MAGENTA-ish
            color = vec3(1.0, 0.15 + 0.35 * uv.y, 1.0);
        } else {
            // -Z face: CYAN-ish
            color = vec3(0.15 + 0.35 * uv.x, 1.0, 1.0);
        }
    }

    // Slight gamma-ish correction to keep colors pleasant
    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}