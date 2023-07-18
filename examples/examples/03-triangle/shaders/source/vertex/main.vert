#version 450

layout (location = 0) out vec2 v_texCoord;

vec2 positions[3] = {
    vec2( 0.0,  0.5),
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5)
};

void main() {
    v_texCoord = positions[gl_VertexIndex] * 0.5 + 0.5;
    v_texCoord.y = 1.0 - v_texCoord.y;
    gl_Position = vec4(positions[gl_VertexIndex], 1.0, 1.0);
}
