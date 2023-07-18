#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

void main() {
    FragColor = vec4(v_texCoord, 0.0, 1.0);
}
