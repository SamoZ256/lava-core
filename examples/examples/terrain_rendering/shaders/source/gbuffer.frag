#version 450

//TODO: remove this hardcoding
#define ALPHA_DISCARD
//#define NORMAL_CORRECTION
//#define DIFFUSE_IN_FIRST_DESCRIPTOR_SET

#ifdef NORMAL_CORRECTION
#define NORMAL normal
#else
#define NORMAL v_normal
#endif

layout (location = 0, color_attachment_index = 1) out vec4 gbufferDiffuse;
layout (location = 1, color_attachment_index = 2) out vec4 gbufferNormal;
#ifdef LV_BACKEND_METAL
//Output the depth into a separate color attachment.
layout (location = 2, color_attachment_index = 3) out float gbufferDepth;
#endif

layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec3 v_normal;

#ifdef DIFFUSE_IN_FIRST_DESCRIPTOR_SET
layout (set = 0, binding = 1)
#else
layout (set = 1, binding = 0)
#endif
uniform sampler2D u_diffuseTexture;

const vec3 lightDirection = normalize(vec3(2.0, -4.0, 1.0));

void main() {
    vec4 diffuseColor = texture(u_diffuseTexture, v_texCoord);
#ifdef ALPHA_DISCARD
    if (diffuseColor.a < 0.5)
        discard;
#endif
#ifdef NORMAL_CORRECTION
    vec3 normal = v_normal;
    if (dot(normal, lightDirection) > 0.5)
        normal = -normal;
#endif
    gbufferDiffuse = vec4(diffuseColor.rgb, 0.0);
    gbufferNormal = vec4(NORMAL, 0.0);
#ifdef LV_BACKEND_METAL
    gbufferDepth = gl_FragCoord.z;
#endif
}
