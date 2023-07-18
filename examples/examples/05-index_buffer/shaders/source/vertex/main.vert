#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

layout (location = 0) out vec3 v_color;

void main() {
    v_color = aColor;
    gl_Position = vec4(aPosition, 1.0);
}
