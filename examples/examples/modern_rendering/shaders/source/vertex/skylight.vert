#version 450

#extension GL_ARB_shader_viewport_layer_array : enable

layout (location = 0) out vec3 v_position;

layout (push_constant) uniform VP {
    mat4 view;
    int layerIndex;
} u_vp;

vec2 positions[3] = {
    vec2(-1.0,  1.0),
    vec2(-1.0, -3.0),
    vec2( 3.0,  1.0)
};

void main() {
    vec4 position = u_vp.view * vec4(positions[gl_VertexIndex], 1.0, 1.0);
    v_position = position.xyz;

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    gl_Layer = u_vp.layerIndex;
}
