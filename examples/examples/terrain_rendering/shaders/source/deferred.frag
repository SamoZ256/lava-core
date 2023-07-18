#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

//TODO: make this a specialization constant
const float FAR_PLANE = 6.5 * 16.0;

//TODO: make this a specialization constant
const uint SHADOW_CASCADE_COUNT = 3;

layout (push_constant) uniform VP {
    mat4 invViewProj;
    mat4 shadowViewProjs[SHADOW_CASCADE_COUNT];
    vec3 viewPos;
} u_vp;

layout (set = 0, binding = 0, input_attachment_index = 0, color_attachment_index = 1) uniform subpassInput u_gbufferDiffuse;
layout (set = 0, binding = 1, input_attachment_index = 1, color_attachment_index = 2) uniform subpassInput u_gbufferNormal;
layout (set = 0, binding = 2, input_attachment_index = 2, color_attachment_index = 3) uniform subpassInput u_gbufferDepth;

layout (set = 0, binding = 3) uniform sampler2DArrayShadow u_shadowTexture;

const float ambient = 0.1;
const float specularStrength = 0.5;

const vec3 lightColor = vec3(2.0, 2.0, 2.0);
const vec3 lightDirection = normalize(vec3(2.0, -4.0, 1.0));

const float PI = 3.14159265359;
#define SHADOW_SAMPLE_COUNT 4
#define SHADOW_PENUMBRA_SIZE 4.0

float fogEnd = FAR_PLANE - 8.0;
float fogBegin = fogEnd - 30.0;
float fogDiff = fogEnd - fogBegin;

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

vec3 phongShading(in vec3 diffuseColor, in vec3 normal, in vec3 position) {
    float diffuse = max(dot(normal, -lightDirection), 0.0);

    vec3 viewDir = normalize(u_vp.viewPos - position);
    vec3 reflectDir = reflect(lightDirection, normal);
    float specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32);

    return (diffuse + specular) * lightColor * diffuseColor;
}

vec3 reconstructPositionFromDepth(in mat4 invViewProj, in vec2 texCoord, in float depth) {
    vec3 posInViewProj = vec3(texCoord * 2.0 - 1.0, depth);
    vec4 position = invViewProj * vec4(posInViewProj, 1.0);
    position.xyz /= position.w;

    return position.xyz;
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

    float bias = 0.05 * tan(acos(dot(normal, lightDirection)));
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

void main() {
    vec3 diffuseColor = subpassLoad(u_gbufferDiffuse).rgb;
    vec3 normal = subpassLoad(u_gbufferNormal).xyz;
    float depth = subpassLoad(u_gbufferDepth).x;
    vec3 position = reconstructPositionFromDepth(u_vp.invViewProj, v_texCoord, depth);

    vec3 result = ambient * diffuseColor * lightColor + phongShading(diffuseColor, normal, position) * getVisibility(position, normal);
    float fogIntensity = min(max(distance(u_vp.viewPos, position) - fogBegin, 0.0) / fogDiff, 1.0);

    FragColor = vec4(result, 1.0 - fogIntensity);
}
