#version 450

#extension GL_ARB_shader_viewport_layer_array : enable

layout (location = 0) in vec3 aPosition;
//layout (location = 1) in vec2 aTexCoord;

//layout (location = 0) out vec2 outTexCoord;

#define CASCADE_COUNT 3

struct Model {
    mat4 model;
    mat4 normalMatrix;
};

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProjs[CASCADE_COUNT];
} u_vp;

layout (set = 1, binding = 0) readonly buffer MODEL {
    Model models[];
} u_model;

void main() {
    gl_Position = u_vp.viewProjs[gl_InstanceIndex % 3] * u_model.models[gl_InstanceIndex / 3].model * vec4(aPosition, 1.0);
    gl_Layer = gl_InstanceIndex % 3;
    //outTexCoord = aTexCoord;
}
