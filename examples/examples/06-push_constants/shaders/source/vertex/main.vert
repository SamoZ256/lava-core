#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

layout (location = 0) out vec3 v_color;

layout (push_constant) uniform POSITION {
    vec3 position;
} u_position;

void main() {
    v_color = aColor;
    gl_Position = vec4(u_position.position + aPosition, 1.0);
}
