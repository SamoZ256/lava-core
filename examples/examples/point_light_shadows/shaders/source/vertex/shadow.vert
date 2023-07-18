#version 450

layout (location = 0) in vec3 aPosition;

layout (location = 0) out vec3 v_position;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
    vec3 lightPos;
} u_vp;

layout (push_constant) uniform MODEL {
    mat4 model;
} u_model;

void main() {
    vec4 position = u_model.model * vec4(aPosition, 1.0);
    v_position = position.xyz;
    gl_Position = u_vp.viewProj * position;
    //gl_Position.xy /= gl_Position.w;
    //gl_Position.z = distance(position.xyz, u_vp.lightPos) / 10.0;
    //gl_Position.w = 1.0;
}
