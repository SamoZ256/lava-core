#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

layout (location = 0) out vec2 v_texCoord;

layout (push_constant) uniform MODEL {
    mat4 model;
} u_model;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
} u_vp;

void main() {
    v_texCoord = aTexCoord;
    gl_Position = u_vp.viewProj * u_model.model * vec4(aPosition, 1.0);
}
