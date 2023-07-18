#version 450

layout (quads, equal_spacing, cw) in;

//layout (location = 0) in vec3 inNormal[];
//layout (location = 1) in vec2 inTexCoord[];

layout (location = 0) out vec2 outTexCoord;
layout (location = 1) out vec2 outTerrainCoord;
layout (location = 2) out vec3 outNormal;

layout (push_constant) uniform MODEL {
    vec3 position;
} u_model;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
} u_vp;

layout (set = 1, binding = 0) uniform sampler2D u_noiseTexture;
layout (set = 1, binding = 1) uniform sampler2D u_normalAOTexture;

void main() {
    vec3 pos1 = mix(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_TessCoord.x);
	vec3 pos2 = mix(gl_in[3].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_TessCoord.x);
	vec3 position = mix(pos1, pos2, gl_TessCoord.y);
    
    outTerrainCoord = position.xz / 16.0;

	position.y = texture(u_noiseTexture, outTerrainCoord).x * 24.0;
    //position.xz *= 2.0;
    //vec3 position = vec3(gl_TessCoord.x * 2.0, textureLod(u_noiseTexture, gl_TessCoord.xy, 0.0).x * 24.0, gl_TessCoord.y * 2.0);

	gl_Position = u_vp.viewProj * vec4(u_model.position + position, 1.0);

    outTexCoord = gl_TessCoord.xy;
    outNormal = texture(u_normalAOTexture, outTerrainCoord).xyz;
}
