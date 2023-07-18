#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 1, color_attachment_index = 4) out vec3 F0;

layout (location = 0) in vec2 v_texCoord;

#define SHADOW_CASCADE_COUNT 3

layout (push_constant) uniform VP {
    mat4 invViewProj;
    mat4 shadowViewProjs[SHADOW_CASCADE_COUNT];
    vec3 viewPos;
} u_vp;

layout (set = 0, binding = 0, input_attachment_index = 0, color_attachment_index = 1) uniform subpassInput u_gbufferAlbedoMetallic;
layout (set = 0, binding = 1, input_attachment_index = 1, color_attachment_index = 2) uniform subpassInput u_gbufferNormalRoughness;
//Depth attachment can be used as input attachment only with the Vulkan backend. Therefore, we need to create a separate color attachment for the depth.
layout (set = 0, binding = 2, input_attachment_index = 2, color_attachment_index = 5) uniform subpassInput u_gbufferDepth;

layout (set = 0, binding = 3) uniform sampler2DArrayShadow u_shadowTexture;

layout (set = 0, binding = 4) uniform DirectLight {
    vec3 color;// = vec3(4.0, 4.0, 4.0);
    vec3 direction;// = normalize(vec3(2.0, -4.0, 1.0));
} u_light;

const float PI = 3.14159265359;
#define SHADOW_SAMPLE_COUNT 4
#define SHADOW_PENUMBRA_SIZE 4.0

vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

//Random number generator
float random(vec3 seed, int i) {
	float dotProduct = dot(vec4(seed, i), vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dotProduct) * 43758.5453);
}

vec3 reconstructPositionFromDepth(in mat4 invViewProj, in vec2 texCoord, in float depth) {
    vec3 posInViewProj = vec3(texCoord * 2.0 - 1.0, depth);
    vec4 position = invViewProj * vec4(posInViewProj, 1.0);
    position.xyz /= position.w;

    return position.xyz;
}

//PBR functions
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = r*r / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

//Direct light
vec3 calcDirectLight(in vec3 norm, in vec3 viewDir, in vec3 F0, in vec3 albedoMetallic, in float roughness, in float metallic) {
    vec3 L = normalize(-u_light.direction);
    vec3 H = normalize(viewDir + L);
    float cosTheta = max(dot(norm, L), 0.0);
    vec3 radiance = vec3(1.0) * cosTheta;

    //NDF and G
    float NDF = DistributionGGX(norm, H, roughness);
    float G = GeometrySmith(norm, viewDir, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0, roughness);

    //Specular
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(norm, viewDir), 0.0) * max(dot(norm, L), 0.0) + 0.0001;
    vec3 spec = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= roughness;

    float NdotL = max(dot(norm, L), 0.0);

    return (kD * albedoMetallic / PI + spec) * radiance * NdotL * u_light.color * 2.0;
}

//Shadows
float getVisibility(in vec3 position, in vec3 normal) {
    int layer = -1;
    vec4 shadowCoord;
    for (int i = 0; i < SHADOW_CASCADE_COUNT; i++) {
        shadowCoord = u_vp.shadowViewProjs[i] * vec4(position, 1.0);
        shadowCoord.xyz /= shadowCoord.w;
        shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
        shadowCoord.y = 1.0 - shadowCoord.y;
        if (shadowCoord.x >= 0.01 && shadowCoord.x <= 0.99 && shadowCoord.y >= 0.01 && shadowCoord.y <= 0.99) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        return 1.0;
    }

    float bias = 0.05 * tan(acos(dot(normal, u_light.direction)));
    bias = clamp(bias, 0.0001, 0.0002);

    float visibility = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_shadowTexture, 0));
    texelSize /= texelSize.x > texelSize.y ? texelSize.x : texelSize.y;

    for (int i = 0; i < SHADOW_SAMPLE_COUNT; i++) {
        vec2 coord = vec2(shadowCoord.xy + normalize(poissonDisk[i]) * random(position, i) / 700.0 * SHADOW_PENUMBRA_SIZE * texelSize);
        visibility += texture(u_shadowTexture, vec4(coord, layer, shadowCoord.z - bias));
    }

    return visibility / SHADOW_SAMPLE_COUNT;
}

/*
vec2 signNotZero(vec2 v) {
    return fma(step(vec2(0.0), v), vec2(2.0), vec2(-1.0));
}

vec3 unpackNormalOctahedron(vec2 packed_nrm) {
    vec3 v = vec3(packed_nrm.xy, 1.0 - abs(packed_nrm.x) - abs(packed_nrm.y));
    if (v.z < 0) v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);

    return normalize(v);
}
*/

void main() {
    vec4 albedoMetallic = subpassLoad(u_gbufferAlbedoMetallic);
    vec4 normalRoughness = subpassLoad(u_gbufferNormalRoughness);
    float depth = subpassLoad(u_gbufferDepth).x;
    vec3 position = reconstructPositionFromDepth(u_vp.invViewProj, v_texCoord, depth);
    
    vec3 normal = normalRoughness.xyz;

    //Camera
    vec3 posToCamera = u_vp.viewPos - position;
    float distToCamera = length(posToCamera);
    vec3 viewDir = posToCamera / distToCamera;

    //F0
    F0 = mix(vec3(0.04), albedoMetallic.rgb, albedoMetallic.a);

    vec3 color = calcDirectLight(normal, viewDir, F0, albedoMetallic.rgb, normalRoughness.a, albedoMetallic.a) * getVisibility(position, normal);
    
    //IBL

    const vec3 skyColor = vec3(0.2);//vec3(47, 166, 222) / 255.0 * 0.2;

    //Diffuse
    vec3 kS = fresnelSchlick(max(dot(normal, viewDir), 0.0), F0, normalRoughness.a);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = skyColor;//texture(u_irradianceMap, normal).rgb;
    vec3 diffuse = irradiance * albedoMetallic.rgb;

    //Specular
    /*
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 R = reflect(viewDir, normal);
    R.y = -R.y;
    vec3 prefilteredColor = skyColor;//textureLod(u_prefilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = vec2(1.0, 0.0);//texture(u_brdfLutMap, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
    */

    vec3 ambient = kD * diffuse/* + specular*/;

    color += ambient;

    FragColor = vec4(color, 1.0);
}
