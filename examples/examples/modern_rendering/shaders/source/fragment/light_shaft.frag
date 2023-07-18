#version 450

#extension GL_EXT_samplerless_texture_functions : require

layout (location = 0) out float FragColor;

layout (location = 0) in vec2 v_texCoord;

#define SHADOW_CASCADE_COUNT 3

layout (push_constant) uniform VP {
    mat4 viewProj;
    mat4 invViewProj;
    mat4 shadowViewProjs[SHADOW_CASCADE_COUNT];
    vec3 viewPos;
} u_vp;

layout (set = 0, binding = 0) uniform texture2D u_gbufferDepth;
layout (set = 0, binding = 1) uniform sampler2DArrayShadow u_shadowTexture;

#define SAMPLE_COUNT 12

vec3 matrixMultiplyProjection(in mat4 invMat, in vec3 vec) {
    vec4 vecInProj = invMat * vec4(vec, 1.0);

    return vecInProj.xyz / vecInProj.w;
}

vec2 texCoordToPos(in vec2 texCoord) {
	return texCoord * vec2(2.0, -2.0) - 1.0;
}

vec2 posToTexCoord(in vec2 pos) {
	return pos * vec2(0.5, -0.5) + 0.5;
}

float getVisibility(in vec3 position) {
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

    float bias = 0.0001;// * tan(acos(dot(normal, u_light.direction)));
    //bias = clamp(bias, 0.001, 0.002);

    return texture(u_shadowTexture, vec4(shadowCoord.xy, layer, shadowCoord.z - bias));
}

const float ditherPattern[4][4] = {
    { 0.0,    0.5,    0.125,  0.625},
    { 0.75,   0.22,   0.875,  0.375},
    { 0.1875, 0.6875, 0.0625, 0.5625},
    { 0.9375, 0.4375, 0.8125, 0.3125}
};

/*
const float PI = 3.141592;
const float G_SCATTERING = 0.01;

float computeScattering(float lightDotView) {
    float result = 1.0 - G_SCATTERING * G_SCATTERING;
    result /= (4.0 * PI * pow(1.0 + G_SCATTERING * G_SCATTERING - (2.0 * G_SCATTERING) * lightDotView, 1.5));

    return result;
}
*/

void main()  {
	vec2 pos = v_texCoord * 2.0 - 1.0;

	vec3 position = matrixMultiplyProjection(u_vp.invViewProj, vec3(pos, texelFetch(u_gbufferDepth, ivec2(gl_FragCoord.xy), 0).x));

    //float scatteringMultiplier = computeScattering(dot(normalize(position - u_vp.viewPos), /*u_vp.lightDirection*/normalize(vec3(3.0, -4.0, 1.0))));

	float stepAmount = 1.0 / SAMPLE_COUNT;
    float visibility = 0.0;
    for (uint i = 0; i < SAMPLE_COUNT; i++) {
        vec3 crntPosition = mix(position, u_vp.viewPos, stepAmount * (i + ditherPattern[uint(gl_FragCoord.x) % 4][uint(gl_FragCoord.y) % 4]));
        visibility += getVisibility(crntPosition);
    }

    FragColor = visibility * stepAmount * 0.1;//scatteringMultiplier;
}
