#version 450

layout (location = 0) out vec2 v_texCoord;

vec2 positions[3] = {
    vec2(-1.0,  1.0),
    vec2(-1.0, -3.0),
    vec2( 3.0,  1.0)
};

void main() {
    v_texCoord = positions[gl_VertexIndex] * 0.5 + 0.5;
    gl_Position = vec4(positions[gl_VertexIndex], 1.0, 1.0);
}
