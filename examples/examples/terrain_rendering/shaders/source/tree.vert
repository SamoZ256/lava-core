#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (location = 0) out vec2 v_texCoord;
layout (location = 1) out vec3 v_normal;

struct Model {
    mat4 model;
    mat4 normalMatrix;
};

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
} u_vp;

layout (set = 0, binding = 1) readonly buffer MODEL {
    Model models[];
} u_model;

void main() {
    v_texCoord = aTexCoord;
    v_normal = normalize(mat3(u_model.models[gl_InstanceIndex].normalMatrix) * aNormal);
    gl_Position = u_vp.viewProj * u_model.models[gl_InstanceIndex].model * vec4(aPosition, 1.0);
}
