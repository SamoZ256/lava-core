#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

layout (location = 0) out vec3 v_color;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
} u_vp;

layout (set = 0, binding = 1) readonly buffer MODEL {
    mat4 models[];
} u_model;

void main() {
    v_color = aColor;
    gl_Position = u_vp.viewProj * u_model.models[gl_InstanceIndex] * vec4(aPosition, 1.0);
}
