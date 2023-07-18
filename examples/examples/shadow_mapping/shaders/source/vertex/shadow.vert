#version 450

layout (location = 0) in vec3 aPosition;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
} u_vp;

layout (push_constant) uniform MODEL {
    mat4 model;
} u_model;

void main() {
    gl_Position = u_vp.viewProj * u_model.model * vec4(aPosition, 1.0);
}
