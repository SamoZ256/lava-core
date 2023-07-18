#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (set = 0, binding = 0, rgba8) uniform readonly image2D u_colorTexture;

const float exposure = 1.0;

void main() {
    vec3 hdrColor = imageLoad(u_colorTexture, ivec2(gl_FragCoord.xy)).rgb;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    FragColor = vec4(mapped, 1.0);
}
