#version 450

layout (location = 0) in vec2 inTexCoord;

layout (set = 2, binding = 0) uniform sampler2D u_diffuseTexture;

void main() {
    if (texture(u_diffuseTexture, inTexCoord).a < 0.5)
        discard;
}
