#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec3 v_color;

void main() {
    FragColor = vec4(v_color, 1.0);
}
