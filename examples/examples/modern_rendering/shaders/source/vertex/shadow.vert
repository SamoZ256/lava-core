#version 450

#extension GL_ARB_shader_viewport_layer_array : enable

layout (location = 0) in vec3 aPosition;

#define CASCADE_COUNT 3

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProjs[CASCADE_COUNT];
} u_vp;

layout (push_constant) uniform MODEL {
    mat4 model;
} u_model;

void main() {
    gl_Position = u_vp.viewProjs[gl_InstanceIndex] * u_model.model * vec4(aPosition, 1.0);
    gl_Layer = gl_InstanceIndex;
}
