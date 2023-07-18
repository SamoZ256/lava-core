#version 450

layout (location = 0) out vec4 gbufferDiffuse;
layout (location = 1) out vec4 gbufferNormal;

layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec3 v_normal;

layout (set = 1, binding = 0) uniform sampler2D u_diffuseTexture;

void main() {
    vec4 diffuseColor = texture(u_diffuseTexture, v_texCoord);
    if (diffuseColor.a < 0.5)
        discard;
    gbufferDiffuse = vec4(diffuseColor.rgb, 0.0);
    gbufferNormal = vec4(v_normal, 0.0);
}
