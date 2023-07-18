#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (set = 1, binding = 0) uniform sampler2D u_colorTexture;

void main() {
    vec4 color = texture(u_colorTexture, v_texCoord);
    if (color.a < 0.5)
        discard;
    FragColor = vec4(color.rgb, 1.0);
}
