#version 450

#extension GL_EXT_samplerless_texture_functions : require

#include "lava_common.glsl"

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

/*layout (constant_id = 0) */const float FAR_PLANE = 100.0; //TODO: make this a specialization constant

///*layout (constant_id = 1) */const int MAX_MIP = 5; //TODO: make this a specialization constant

layout (push_constant) uniform VP {
    mat4 viewProj;
    mat4 invViewProj;
    mat4 view;
    vec3 viewPos;
    uint frameIndex;
} u_vp;

//layout (set = 0, binding = 0) uniform sampler2D u_colorTexture;
layout (set = 0, binding = 0) uniform sampler2D u_gbufferDepth;
layout (set = 0, binding = 1) uniform texture2D u_gbufferNormalRoughness;

vec3 reconstructPositionFromDepth(in mat4 invViewProj, in vec2 texCoord, in float depth) {
    vec3 posInViewProj = vec3(texCoord * 2.0 - 1.0, depth);
    vec4 position = invViewProj * vec4(posInViewProj, 1.0);
    position.xyz /= position.w;

    return position.xyz;
}

const float rayStep = 0.8;
const float minRayStep = 0.5;
const float maxSteps = 30;
const int numBinarySearchSteps = 10;
float tolerance = 1.0 / FAR_PLANE * 0.002;

const float borderFog = 0.03;

vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord) {
    float depth = 0.0;
    vec4 projectedCoord;
    float dDepth = 0.0;

    for (int i = 0; i < numBinarySearchSteps; i++) {
        projectedCoord = u_vp.viewProj * vec4(hitCoord, 1.0);
        projectedCoord.xyz /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        projectedCoord.y = 1.0 - projectedCoord.y;

        depth = texture(u_gbufferDepth, projectedCoord.xy).x;

        dDepth = projectedCoord.z - depth;
        dir *= 0.5;

        if (dDepth > 0.0)
            hitCoord -= dir;
        else
            hitCoord += dir;
    }

    projectedCoord = u_vp.viewProj * vec4(hitCoord, 1.0);
    projectedCoord.xyz /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
    projectedCoord.y = 1.0 - projectedCoord.y;

    return abs(dDepth) < tolerance ? vec3(projectedCoord.xy, depth) : vec3(-1);
}

vec3 rayMarch(vec3 dir, inout vec3 hitCoord) {
    dir *= rayStep;
    float depth = 0.0;
    vec4 projectedCoord;
    //float dDepth = 0.0;

    for (int i = 0; i < maxSteps; i++) {
        hitCoord += dir;

        projectedCoord = u_vp.viewProj * vec4(hitCoord, 1.0);
        projectedCoord.xyz /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        projectedCoord.y = 1.0 - projectedCoord.y;
        //projectedCoord.z = abs((u_vp.view * vec4(reconstructPosFromDepth(projectedCoord.z), 1.0)).z);
        //projectedCoord.z /= 2.0;

        depth = texture(u_gbufferDepth, projectedCoord.xy).x;
        //depth = abs((u_vp.view * vec4(reconstructPosFromDepth(depth), 1.0)).z);
        //return vec3(depth);
        //depth = (u_vp.view * vec4(reconstructPosFromDepth(depth), 1.0)).z;
        if (depth == 1.0)
            continue;

        if (projectedCoord.z >= depth)
            return binarySearch(dir, hitCoord);
    }

    return vec3(-1);//vec3(projectedCoord.xy, depth);
}

#define K 19.19

vec3 hash(vec3 a) {
   vec3 p3 = fract(a * 0.8);
   p3 += dot(p3, p3.yzx+K);

   return fract((p3.xxy+p3.yzz)*p3.zyx) * 2.0 - 1.0; 
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
    vec4 normalRoughness = texelFetch(u_gbufferNormalRoughness, ivec2(gl_FragCoord.xy * 2), 0);
    float depth = texture(u_gbufferDepth, vec2(v_texCoord.x, 1.0 - v_texCoord.y)).x;
    vec3 position = reconstructPositionFromDepth(u_vp.invViewProj, v_texCoord, depth);
    vec3 normal = normalRoughness.xyz;

    vec3 reflected = reflect(normalize(position - u_vp.viewPos), normal);
    vec3 hitPos = position;

    //Ray marching
    float trueDepth = abs((u_vp.view * vec4(position, 1.0)).z);

    //vec3 wp = /*vec3(vec4(*/u_vp.viewPos/*, 1.0) * invView)*/;
    vec3 jitt = mix(vec3(0.0), vec3(hash(position * (u_vp.frameIndex + 1))), normalRoughness.a + 0.04) * 0.1;
    vec3 coords = rayMarch(normalize(reflected + jitt) * max(minRayStep, min(trueDepth * 0.2, 2.0)), hitPos);
    //FragColor = vec4(color * 0.001 + texture(u_colorTexture, coords.xy).rgb, 1.0);

    vec3 hitVector = vec3(0.0);

    if (coords.x >= 0.0 && coords.x <= 1.0 && coords.y >= 0.0 && coords.y <= 1.0) {
        //float borderX = coords.x < 0.5 ? max(borderFog - coords.x, 0.0001) / borderFog : max(coords.x - (1.0 - borderFog), 0.0001) / borderFog;
        //float borderY = coords.y < 0.5 ? max(borderFog - coords.y, 0.0001) / borderFog : max(coords.y - (1.0 - borderFog), 0.0001) / borderFog;
        //float strength = clamp(1.0 - (borderX + borderY), 0.0, 1.0) * 0.8;
        //reflectedColor = vec4(texture/*Lod*/(u_colorTexture, coords.xy/*, normalRoughness.a * MAX_MIP*/).rgb, 1.0);
        //prefilteredColor = texture/*Lod*/(u_colorTexture, coords.xy).rgb;
        hitVector = hitPos - position;
    }

    FragColor = vec4(hitVector, 0.0);//vec4(specular * (normalRoughness.b * 0.4 + 0.6), 0.0);

    //FragColor = vec4(color + position * normal * 0.001, 1.0);
}
