#version 450

layout (location = 0, color_attachment_index = 1) out vec4 gbufferAlbedoMetallic;
layout (location = 1, color_attachment_index = 2) out vec4 gbufferNormalRoughness;
layout (location = 2, color_attachment_index = 3) out vec2 gbufferMotion;
#ifdef LV_BACKEND_METAL
//Output the depth into a separate color attachment.
layout (location = 3, color_attachment_index = 5) out float gbufferDepth;
#endif

layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec4 v_pos;
layout (location = 2) in vec4 v_prevPos;
layout (location = 3) in mat3 v_TBN;

layout (set = 1, binding = 0) uniform sampler2D u_albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D u_metallicRoughnessTexture;
layout (set = 1, binding = 2) uniform sampler2D u_normalTexture;

/*
vec2 sign_not_zero(vec2 v) {
    return fma(step(vec2(0.0), v), vec2(2.0), vec2(-1.0));
}

vec2 packNormalOctahedron(vec3 v) {
    v.xy /= dot(abs(v), vec3(1));

    return mix(v.xy, (1.0 - abs(v.yx)) * sign_not_zero(v.xy), step(v.z, 0.0));
}
*/

void main() {
    vec4 albedo = texture(u_albedoTexture, v_texCoord);
    if (albedo.a < 0.5)
        discard;
    vec2 metallicRoughness = texture(u_metallicRoughnessTexture, v_texCoord).rg;

    vec3 localNormal = texture(u_normalTexture, v_texCoord).xyz;
    localNormal = normalize(localNormal * 2.0 - 1.0);
    //localNormal.y = 1.0 - localNormal.y;
    vec3 normal = normalize(v_TBN * localNormal);
    //if ((v_TBN * vec3(0.0, 0.0, 1.0)).y == 1.0)
    //    albedo.rgb *= 0.1;

    gbufferAlbedoMetallic = vec4(albedo.rgb, metallicRoughness.r);
    gbufferNormalRoughness = vec4(normal, metallicRoughness.g);
    gbufferMotion = v_prevPos.xy / v_prevPos.w - v_pos.xy / v_pos.w;//v_motion;
#ifdef LV_BACKEND_METAL
    gbufferDepth = gl_FragCoord.z;
#endif
}
