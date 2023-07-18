#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (set = 0, binding = 1) uniform sampler2D u_colorTexture;

void main() {
    FragColor = vec4(texture(u_colorTexture, v_texCoord).rgb, 1.0);
}
