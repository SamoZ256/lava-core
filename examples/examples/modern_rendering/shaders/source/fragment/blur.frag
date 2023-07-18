#version 450

#extension GL_EXT_samplerless_texture_functions : require

#define BLUR_RANGE 2

layout (location = 0) out float FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (set = 0, binding = 0) uniform texture2D u_texture;

void main() {
    FragColor = 0.0;
	int n = 0;
	//vec2 texelSize = 1.0 / vec2(textureSize(u_texture, 0));
    //float initValue = imageLoad(u_texture, ivec2(gl_FragCoord.xy)).r;
	for (int x = -BLUR_RANGE; x <= BLUR_RANGE; x++) {
		for (int y = -BLUR_RANGE; y <= BLUR_RANGE; y++) {
			//vec2 offset = vec2(x, y) * texelSize;
            FragColor += texelFetch(u_texture, ivec2(gl_FragCoord.xy) + ivec2(x, y), 0).r;
			n++;
		}
	}
	
	FragColor /= float(n);
}
