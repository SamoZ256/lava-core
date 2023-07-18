#version 450

layout (location = 0) in vec3 aPosition;

layout (location = 0) out vec3 v_position;
//layout (location = 1) out vec4 v_crntPosition;
//layout (location = 2) out vec4 v_prevPosition;

layout (push_constant) uniform VP {
    mat4 viewProj;
    mat4 prevViewProj;
} u_vp;

void main() {
    v_position = aPosition;

    //v_crntPosition = u_vp.viewProj * vec4(aPosition, 1.0);
    //v_prevPosition = u_vp.prevViewProj * vec4(aPosition, 1.0);

    gl_Position = u_vp.viewProj * vec4(aPosition * 100.0, 1.0);
    //gl_Position.xy /= gl_Position.w;
    //gl_Position.z = 1.0;
    //gl_Position.w = 1.0;
}
