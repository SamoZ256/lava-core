#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (set = 0, binding = 0) uniform sampler2D u_colorTexture;
layout (set = 0, binding = 1, rg16) uniform readonly image2D u_motionTexture;

layout (push_constant) uniform DELTA_TIME {
    float dt;
} u_dt;

const float BLUR_SIZE = 1.0;
const float TARGET_FPS = 60.0;
const uint SAMPLE_COUNT = 16;

/*
const float WEIGHTS[SAMPLE_COUNT] = {
    1.0,
    0.9,
    0.7,
    0.051,
	0.0918,
	0.12245,
	0.1531,
	0.1633,
	0.1531,
	0.12245,
	0.0918,
	0.051
};
*/

vec2 posToTexCoord(in vec2 pos) {
	return pos * vec2(0.5, -0.5) + 0.5;
}

void main() {
    vec2 texCoord = v_texCoord;
    texCoord.y = 1.0 - texCoord.y;

    float stepSize = BLUR_SIZE / TARGET_FPS / SAMPLE_COUNT / u_dt.dt;
    vec2 blurDir = imageLoad(u_motionTexture, ivec2(gl_FragCoord.xy)).rg * 0.5 * stepSize;

    vec3 color = vec3(0.0);
    float weight = 0.0;

    for (uint i = 0; i < SAMPLE_COUNT; i++) {
        vec2 crntTexCoord = texCoord + blurDir * i;
        //if (crntTexCoord.x >= 0.0 && crntTexCoord.x <= 1.0 && crntTexCoord.y >= 0.0 && crntTexCoord.y <= 1.0) {
        float crntWeight = 1.0 / (i + 1.0);
        color += texture(u_colorTexture, crntTexCoord).rgb * crntWeight;
        weight += crntWeight;
        //}
    }

    FragColor = vec4(color / weight, 0.0);
}
