#version 450

layout (location = 0) out vec3 v_position;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
    vec3 lightPos;
} u_vp;

void main() {
    gl_FragDepth = distance(v_position, u_vp.lightPos) / 10.0;
}
