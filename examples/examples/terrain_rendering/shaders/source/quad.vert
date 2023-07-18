#version 450

layout (location = 0) out vec2 v_texCoord;
layout (location = 1) out vec3 v_normal;

struct Model {
    mat4 model;
    mat4 normalMatrix;
};

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
} u_vp;

layout (set = 1, binding = 0) readonly buffer MODELS {
    Model models[];
} u_model;

vec2 positions[] = {
    {-1.0, -1.0},
    { 1.0, -1.0},
    { 1.0,  1.0},
    {-1.0,  1.0}
};

void main() {
    v_texCoord = positions[gl_VertexIndex] * 0.5 + 0.5;
    v_texCoord.y = 1.0 - v_texCoord.y;
    v_normal = normalize(mat3(u_model.models[gl_InstanceIndex].normalMatrix) * normalize(vec3(0.0, 2.0, 1.0)));
    gl_Position = u_vp.viewProj * u_model.models[gl_InstanceIndex].model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
