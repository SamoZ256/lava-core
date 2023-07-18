#version 450

layout (vertices = 4) out;

//TODO: make this a specialization constant
const uint maxTessellationFactor = 16;

layout (set = 0, binding = 0) uniform CAMERA {
    vec2 cameraPos;
} u_camera;

layout (push_constant) uniform POSITION {
    vec3 chunkPosition;
} u_position;

float calculateTessFactor(vec2 position) {
    return 1.0 / (1.0 + distance(u_camera.cameraPos, position) * 0.2) * maxTessellationFactor;
}

void main() {
	if (gl_InvocationID == 0) {
        uint x = gl_PrimitiveID % 16;
        uint z = gl_PrimitiveID / 16;
        vec2 position = u_position.chunkPosition.xz + vec2(x, z);

        float f0 = calculateTessFactor(position + vec2(0.0, 0.5));
        float f1 = calculateTessFactor(position + vec2(0.5, 0.0));
        float f2 = calculateTessFactor(position + vec2(1.0, 0.5));
        float f3 = calculateTessFactor(position + vec2(0.5, 1.0));

        gl_TessLevelOuter[0] = f0;
        gl_TessLevelOuter[1] = f1;
        gl_TessLevelOuter[2] = f2;
        gl_TessLevelOuter[3] = f3;

        gl_TessLevelInner[0] = (f1 + f3) * 0.5;
        gl_TessLevelInner[1] = (f0 + f2) * 0.5;
	}
}
