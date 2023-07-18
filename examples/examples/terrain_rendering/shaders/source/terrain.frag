#version 450

layout (location = 0, color_attachment_index = 1) out vec4 gbufferDiffuse;
layout (location = 1, color_attachment_index = 2) out vec4 gbufferNormal;
#ifdef LV_BACKEND_METAL
//Output the depth into a separate color attachment.
layout (location = 2, color_attachment_index = 3) out float gbufferDepth;
#endif

layout (location = 0) in vec2 inTexCoord;
layout (location = 1) in vec2 inTerrainCoord;
layout (location = 2) in vec3 inNormal;

layout (set = 0, binding = 1) uniform sampler2D u_diffuseTexture;

layout (set = 1, binding = 2) uniform sampler2D u_normalAOTexture;

void main() {
    vec3 diffuseColor = texture(u_diffuseTexture, inTexCoord).rgb;
    gbufferDiffuse = vec4(diffuseColor.rgb, 0.0) * texture(u_normalAOTexture, inTerrainCoord).a;
    gbufferNormal = vec4(inNormal, 0.0);
#ifdef LV_BACKEND_METAL
    gbufferDepth = gl_FragCoord.z;
#endif
}
