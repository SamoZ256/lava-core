#version 450

layout (location = 0, color_attachment_index = 1) out vec4 gbufferDiffuse;
layout (location = 1, color_attachment_index = 2) out vec4 gbufferNormal;
#ifdef LV_BACKEND_METAL
//Output the depth into a separate color attachment.
layout (location = 2, color_attachment_index = 3) out float gbufferDepth;
#endif

layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec3 v_normal;

layout (set = 0, binding = 1) uniform sampler2D u_diffuseTexture;

const vec3 lightDirection = normalize(vec3(2.0, -4.0, 1.0));

void main() {
    vec4 diffuseColor = texture(u_diffuseTexture, v_texCoord);
    if (diffuseColor.a < 0.5)
        discard;
    vec3 normal = v_normal;
    if (dot(normal, lightDirection) > 0.5)
        normal = -normal;
    gbufferDiffuse = vec4(diffuseColor.rgb * (0.5 + min(v_texCoord.y * 10.0, 1.0) * 0.5), 0.0);
    gbufferNormal = vec4(normal, 0.0);
#ifdef LV_BACKEND_METAL
    gbufferDepth = gl_FragCoord.z;
#endif
}
