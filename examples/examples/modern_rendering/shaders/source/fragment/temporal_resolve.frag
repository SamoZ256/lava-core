#version 450

#extension GL_EXT_samplerless_texture_functions : require

#define BLUR_RANGE 2

layout (location = 0, color_attachment_index = 1) out vec3 resolvedSSR;
layout (location = 1, color_attachment_index = 2) out float resolvedSSAO;
layout (location = 2, color_attachment_index = 3) out vec4 resolvedColor;
layout (location = 3, color_attachment_index = 4) out vec4 finalColor;

layout (location = 0) in vec2 v_texCoord;

layout (push_constant) uniform VP {
	//mat4 projection;
	//mat4 prevInvProj;
	mat4 viewProj;
	mat4 invViewProj;
	//mat4 prevInvViewProj;
	//mat4 invView;
	//mat4 invProj;
	//mat4 prevView;
	vec4 lightColor;
	vec3 viewPos;
	float reflectionTemporalDivider;
	uint frameIndex;
	bool useSpatialFilter;
} u_vp;

layout (set = 0, binding = 0) uniform sampler2D u_colorTexture;
layout (set = 0, binding = 1) uniform sampler2D u_prevSsrTexture;
layout (set = 0, binding = 2) uniform sampler2D u_prevSsaoTexture;
layout (set = 0, binding = 3) uniform sampler2D u_prevColorTexture;
layout (set = 0, binding = 4) uniform sampler2D u_brdfLookupTexture;

layout (set = 1, binding = 0) uniform texture2D u_depthTexture;
layout (set = 1, binding = 1) uniform texture2D u_gbufferNormalRoughness;
layout (set = 1, binding = 2) uniform texture2D u_motionTexture;
layout (set = 1, binding = 3) uniform texture2D u_f0Texture;

layout (set = 2, binding = 0) uniform texture2D u_ssrHitTexture;
layout (set = 2, binding = 1) uniform texture2D u_lightShaftTexture;
layout (set = 2, binding = 2, input_attachment_index = 0, color_attachment_index = 0) uniform subpassInput u_ssaoTexture;

const float INFINITY = 9999.0;

const uint NEIGHBOUR_SAMPLE_COUNT = 4;
const ivec2 NEIGHBOUR_SAMPLES[NEIGHBOUR_SAMPLE_COUNT] = {
	//{0, 0},
	{-1, 0}, {0, -1}, {1, 0}, {0, 1},
	//{-1, 1}, {-1, -1}, {1, -1}, {1, 1},
	//{-2, 1}, {-1, -2}, {2, -1}, {1, 2}
};

vec3 matrixMultiplyProjection(in mat4 invMat, in vec3 vec) {
    vec4 vecInProj = invMat * vec4(vec, 1.0);

    return vecInProj.xyz / vecInProj.w;
}

vec2 texCoordToPos(in vec2 texCoord) {
	return texCoord * vec2(2.0, -2.0) - 1.0;
}

vec2 posToTexCoord(in vec2 pos) {
	return pos * vec2(0.5, -0.5) + 0.5;
}

/*
vec2 signNotZero(vec2 v) {
    return fma(step(vec2(0.0), v), vec2(2.0), vec2(-1.0));
}

vec3 unpackNormalOctahedron(vec2 packed_nrm) {
    vec3 v = vec3(packed_nrm.xy, 1.0 - abs(packed_nrm.x) - abs(packed_nrm.y));
    if (v.z < 0) v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);

    return normalize(v);
}

vec3 projPointInPlane(in vec3 p, in vec3 v0, in vec3 n, out float d) {
	d = dot(n, p - v0);

	return p - (n * d);
}

vec3 findReflectionIncidentPoint(vec3 p0, vec3 p1, vec3 v0, vec3 n) {
	float d0 = 0;
	float d1 = 0;
	vec3 proj_p0 = projPointInPlane(p0, v0, n, d0);
	vec3 proj_p1 = projPointInPlane(p1, v0, n, d1);

	if(d1 < d0)
		return (proj_p0 - proj_p1) * d1/(d0+d1) + proj_p1;
	else
		return (proj_p1 - proj_p0) * d0/(d0+d1) + proj_p0;
}
*/

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()  {
	vec2 pos = v_texCoord * 2.0 - 1.0;

	/*
	float lightShaft = 0.0;
	int n = 0;
	//vec2 texelSize = 1.0 / vec2(textureSize(u_texture, 0));
    //float initValue = imageLoad(u_texture, ivec2(gl_FragCoord.xy)).r;
	for (int x = -BLUR_RANGE; x <= BLUR_RANGE; x++) {
		for (int y = -BLUR_RANGE; y <= BLUR_RANGE; y++) {
			//vec2 offset = vec2(x, y) * texelSize;
			if (u_vp.frameIndex > 0 || (x == 0 && y == 0)) {
            lightShaft += imageLoad(u_lightShaftTexture, ivec2(gl_FragCoord.xy / 2) + ivec2(x, y)).r;
			n++;
			}
		}
	}
	
	lightShaft /= float(n);
	*/

    vec4 normalRoughness = texelFetch(u_gbufferNormalRoughness, ivec2(gl_FragCoord.xy), 0);
    vec3 normal = normalRoughness.xyz;
	float depth = texelFetch(u_depthTexture, ivec2(gl_FragCoord.xy), 0).x;
	vec3 position = matrixMultiplyProjection(u_vp.invViewProj, vec3(pos, depth));

	//vec3 useless = (texture(u_colorTexture, vec2(0.0)).rgb + texture(u_prevDepthTexture, vec2(0.0)).rgb) * 0.0;

	vec3 hitVector = texelFetch(u_ssrHitTexture, ivec2(gl_FragCoord.xy / 2), 0).xyz;
	vec3 hitPosition = position + hitVector;
	vec3 centerColor = texture(u_colorTexture, posToTexCoord(matrixMultiplyProjection(u_vp.viewProj, hitPosition).xy)).rgb;

    //IBL
    const vec3 skyColor = vec3(0.2);//vec3(47, 166, 222) / 255.0 * 0.2;

	vec3 F0 = texelFetch(u_f0Texture, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 viewDir = normalize(u_vp.viewPos - position);
    vec3 kS = fresnelSchlick(max(dot(normal, viewDir), 0.0), F0, normalRoughness.a);

    //const float MAX_REFLECTION_LOD = 4.0;
    vec3 R = reflect(u_vp.viewPos, normal);
    R.y = -R.y;
    vec3 prefilteredColor = skyColor * 0.1;//textureLod(u_prefilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;

	vec2 motionVector = texelFetch(u_motionTexture, ivec2(gl_FragCoord.xy), 0).xy;
	vec2 prevPos = pos + motionVector;
	float reflectionMotionMultiplier = 1.0;
	float hitVectorLength = length(hitVector);
	if (hitVectorLength > 0.0) {
		//vec3 reflectedColor = centerColor;
		//float weight = 1.0;
		//if (u_vp.useSpatialFilter) {
		//	for (uint i = 0; i < NEIGHBOUR_SAMPLE_COUNT; i++) {
		//		vec3 crntHitVector = imageLoad(u_ssrHitTexture, ivec2(gl_FragCoord.xy / 2 + NEIGHBOUR_SAMPLES[i]/* * (normalRoughness.a + 1)*/)).xyz;
		//		if (dot(crntHitVector, crntHitVector) > 0.0) {
		//			vec3 crntHitPosition = position + crntHitVector;
		//			vec3 projectedHit = matrixMultiplyProjection(u_vp.viewProj, crntHitPosition);
		//			vec3 crntColor = texture(u_colorTexture, posToTexCoord(projectedHit.xy)).rgb;
		//			float crntWeight = 1.0 / (dot(centerColor, crntColor) * 0.4 + 1.0);
		//			reflectedColor += crntColor * crntWeight;
		//			weight += crntWeight;
		//		}
		//	}
		//}
		
		/*
		vec3 projectedPrevHit;
		projectedPrevHit.xy = projectedHit.xy + texture(u_motionTexture, projectedHit.xy).xy;
		projectedPrevHit.z = texture(u_prevDepthTexture, projectedPrevHit.xy).x;

		vec3 view_n = normalize((u_vp.prevView * vec4(normal, 0)).xyz);
		vec3 view_p0 = vec3(0.0);
		vec3 view_v0 = matrixMultiplyProjection(u_vp.prevInvProj, vec3(prevPos, texture(u_prevDepthTexture, posToTexCoord(prevPos)).x));
		vec3 view_p1 = matrixMultiplyProjection(u_vp.prevInvProj, projectedPrevHit.xyz);

		vec3 prevIncidencePoint = (inverse(u_vp.prevView) * vec4(findReflectionIncidentPoint(view_p0, view_p1.xyz, view_v0.xyz, view_n), 1.0)).xyz;

		vec4 projectedPrevIncidencePoint = u_vp.projection * vec4(prevIncidencePoint, 1.0);
		projectedPrevIncidencePoint.xy /= projectedPrevIncidencePoint.w;
		prevPos = projectedPrevIncidencePoint.xy;
		*/

		prefilteredColor = centerColor;//reflectedColor / weight;//texture(u_colorTexture, posToTexCoord(projectedHit.xy)).rgb;
		reflectionMotionMultiplier = 1.0 / (1.0 + hitVectorLength / distance(position, u_vp.viewPos) * u_vp.reflectionTemporalDivider);
	}

    //prefilteredColor = mix(prefilteredColor, reflectedColor.rgb, reflectedColor.a);
    vec2 brdf = texture(u_brdfLookupTexture, vec2(max(dot(normal, viewDir), 0.0), normalRoughness.a)).rg;
    resolvedSSR = vec3(0.0);
	resolvedSSAO = 1.0;
	resolvedColor = vec4(texture(u_colorTexture, posToTexCoord(pos)).rgb, 0.0);

	vec3 lightShaft = vec3(0.0);
	if (depth != 1.0) {
    	resolvedSSR = prefilteredColor;
		resolvedSSAO = subpassLoad(u_ssaoTexture).r;
		lightShaft = texelFetch(u_lightShaftTexture, ivec2(gl_FragCoord.xy / 2), 0).r * u_vp.lightColor.rgb * u_vp.lightColor.a;
		if (u_vp.frameIndex > 0) {
			if (prevPos.x >= -1.0 && prevPos.x <= 1.0 && prevPos.y >= -1.0 && prevPos.y <= 1.0) {
				vec3 minColor = vec3(INFINITY), maxColor = vec3(-INFINITY);
	
				// Sample a 3x3 neighborhood to create a box in color space
				for (int x = -1; x <= 1; ++x) {
					for (int y = -1; y <= 1; ++y) {
						vec3 color = texture(u_colorTexture, posToTexCoord(pos) + vec2(x, y) / textureSize(u_colorTexture, 0)).rgb;
						minColor = min(minColor, color);
						maxColor = max(maxColor, color);
					}
				}
				
				vec3 prevSSR = texture(u_prevSsrTexture, posToTexCoord(pos + motionVector * reflectionMotionMultiplier)).rgb;
				resolvedSSR = mix(resolvedSSR, prevSSR, 0.95);
				float prevSSAO = texture(u_prevSsaoTexture, posToTexCoord(prevPos)).r;
				resolvedSSAO = mix(resolvedSSAO, prevSSAO, 0.95);
				vec3 prevColor = texture(u_prevColorTexture, posToTexCoord(prevPos)).rgb;
				prevColor = clamp(prevColor, minColor, maxColor);
				resolvedColor.rgb = mix(resolvedColor.rgb, prevColor, 0.9);
			}
		}
	}

	finalColor = vec4((resolvedColor.rgb + resolvedSSR * (kS * brdf.r + brdf.g * 3.0)) * resolvedSSAO + lightShaft, 0.0);
}
