#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (location = 0) out vec2 v_texCoord;
layout (location = 1) out vec3 v_normal;
layout (location = 2) out vec3 v_position;

layout (push_constant) uniform MODEL {
    mat4 model;
    mat4 normalMatrix;
} u_model;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
    vec3 viewPos;
} u_vp;

void main() {
    v_texCoord = aTexCoord;
    v_normal = normalize(mat3(u_model.normalMatrix) * aNormal);
    vec4 positionInWorldSpace = u_model.model * vec4(aPosition, 1.0);
    v_position = positionInWorldSpace.xyz;
    gl_Position = u_vp.viewProj * positionInWorldSpace;
}
