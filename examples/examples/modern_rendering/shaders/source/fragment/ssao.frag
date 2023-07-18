#version 450

#extension GL_EXT_samplerless_texture_functions : require

layout (location = 0) out float FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (set = 0, binding = 0) uniform sampler2D u_depthTexture;
layout (set = 0, binding = 1) uniform texture2D u_normalRoughnessTexture;
layout (set = 0, binding = 2) uniform sampler2D u_ssaoNoiseTexture;

layout (push_constant) uniform VP  {
	mat4 projection;
    mat4 view;
    mat4 invViewProj;
    uint frameIndex;
} u_vp;

const float PI = 3.141592;
const float INFINITY = 1.0 / 0.0;

const uint SSAO_SAMPLE_SET_COUNT = 16;
const uint SSAO_KERNEL_SIZE = 8;
const float SSAO_RADIUS = 0.8;
const float HBAO_RADIUS = 40.0;
//const float RANGE_CHECK_RADIUS = 0.02;

const vec3 SSAO_KERNEL[SSAO_SAMPLE_SET_COUNT][SSAO_KERNEL_SIZE] = {
  {
      {0.0532538, -0.0011369, 0.0295376},
      {0.0239844, 0.061514, 0.0641257},
      {-0.0451197, 0.0387557, 0.0290932},
      {0.00446408, -0.0161756, 0.0325572},
      {0.129614, -0.24337, 0.0255956},
      {0.226731, -0.0337583, 0.113058},
      {0.0429641, -0.175016, 0.388265},
      {-0.308357, 0.46751, 0.216027},
  },
  {
      {0.048364, 0.0399839, 0.00833059},
      {0.0321694, 0.0223933, 0.0832177},
      {-0.00339669, -0.0652705, 0.0140811},
      {0.0859671, -0.0067622, 0.10193},
      {-0.201599, -0.151676, 0.176162},
      {0.306923, -0.305096, 0.00293169},
      {0.153313, -0.295194, 0.0118072},
      {0.0113466, -0.00883502, 0.0135629},
  },
  {
      {0.0175278, 0.0777427, 0.0346845},
      {0.0108447, -0.0322699, 0.0158454},
      {-0.00999419, 0.00996832, 0.0196455},
      {-0.0727428, 0.131563, 0.147997},
      {-0.0387771, 0.189173, 0.146183},
      {0.00927954, -0.00831919, 0.0266484},
      {0.229763, 0.188953, 0.26819},
      {0.351743, -0.195922, 0.62465},
  },
  {
      {0.00316157, 0.00840392, 0.00214133},
      {0.0292065, 0.0185952, 0.027604},
      {0.00531828, -0.0856647, 0.129843},
      {0.0252258, -0.115512, 0.0304155},
      {0.039667, 0.0195254, 0.0399306},
      {-0.0520884, 0.25325, 0.262939},
      {0.274127, 0.307869, 0.208575},
      {0.0238985, 0.293917, 0.537027},
  },
  {
      {-0.0227356, 0.00824274, 0.0530448},
      {0.0808462, 0.0244845, 0.056395},
      {-0.0469512, 0.0734431, 0.00120586},
      {0.106925, 0.042372, 0.0369166},
      {-0.129462, 0.0865346, 0.0347488},
      {-0.159513, 0.255782, 0.0689171},
      {-0.296383, 0.177974, 0.189019},
      {-0.229275, -0.134907, 0.0935149},
  },
  {
      {0.0236197, 0.0338698, 0.0174662},
      {-0.00836952, -0.00631819, 0.00613634},
      {0.105748, -0.0801954, 0.0160957},
      {-0.00633416, 0.0289235, 0.0307474},
      {0.0676296, 0.161967, 0.213846},
      {-0.122284, -0.181261, 0.188794},
      {0.183303, 0.409003, 0.230089},
      {-0.0477223, 0.0573987, 0.30914},
  },
  {
      {0.00529578, -0.00271057, 0.00385515},
      {0.0622631, -0.0603817, 0.0538637},
      {0.00289755, 0.0199282, 0.0320573},
      {0.00401005, 0.0879558, 0.0891431},
      {-0.0953868, -0.0632651, 0.0330821},
      {-0.110525, 0.126456, 0.0103576},
      {-0.0540502, 0.461175, 0.0176338},
      {0.337184, 0.463714, 0.450672},
  },
  {
      {0.0252572, 0.0149515, 0.0161187},
      {0.00344782, -0.0036255, 0.00264535},
      {-0.0015302, 0.00061396, 0.00105585},
      {0.169974, -0.0289006, 0.0644733},
      {-0.0538588, 0.226099, 0.111028},
      {0.0431245, 0.0868968, 0.0552407},
      {-0.0044373, -0.086126, 0.268822},
      {0.313962, 0.449455, 0.26912},
  },
  {
      {0.0314273, -0.0130614, 0.0184785},
      {0.00379765, -0.0137521, 0.00874156},
      {0.0120779, 0.0125734, 0.00983059},
      {-0.0339166, -0.0557231, 0.0136768},
      {0.000620368, -0.00107808, 0.000902088},
      {-0.097579, 0.158168, 0.137605},
      {-0.0558147, 0.234971, 0.219976},
      {0.0751242, 0.268367, 0.332562},
  },
  {
      {-0.0238772, 0.0323695, 0.0279066},
      {0.0040162, -0.0338067, 0.0383102},
      {-0.0382632, -0.0935822, 0.0477238},
      {0.0193598, 0.0234826, 0.0242562},
      {0.112862, -0.149785, 0.204885},
      {0.122762, -0.156748, 0.156546},
      {-0.0770277, -0.127026, 0.0427407},
      {-0.511922, 0.350782, 0.444498},
  },
  {
      {-0.0327165, 0.0143366, 0.0414677},
      {-0.0729998, -0.0471025, 0.0297687},
      {0.0377326, 0.0413062, 0.0998259},
      {0.00842264, -0.00360797, 0.0204022},
      {-0.0489581, -0.039387, 0.0106863},
      {0.0350185, -0.0653023, 0.110674},
      {0.174536, 0.0093276, 0.554892},
      {0.0679614, 0.0463061, 0.0896332},
  },
  {
      {0.00445615, 0.017923, 0.000380038},
      {0.0697486, 0.0235892, 0.0314194},
      {-0.0169755, -0.0173283, 0.0139606},
      {-0.000853614, 0.0177963, 0.00996222},
      {-0.278748, -0.0033116, 0.112173},
      {0.0335567, -0.0604204, 0.00787968},
      {0.0668458, 0.0519878, 0.064085},
      {0.0836915, 0.141594, 0.0116753},
  },
  {
      {0.0467917, 0.00570678, 0.0335032},
      {0.0400909, 0.0148197, 0.0179232},
      {-0.0294332, 0.0433921, 0.11057},
      {0.155387, -0.133119, 0.0243763},
      {-0.121237, -0.150058, 0.20956},
      {0.0700185, -0.261231, 0.233469},
      {0.272191, 0.262927, 0.143473},
      {-0.0881208, 0.0576237, 0.0219461},
  },
  {
      {0.0199414, 0.0265795, 0.0452783},
      {-0.00560084, 0.0110316, 0.0132779},
      {-0.0186622, 0.0695342, 0.0246076},
      {-0.013096, 0.00594559, 0.0114093},
      {-0.0733305, 0.298001, 0.0778745},
      {0.0718102, -0.0810926, 0.00673495},
      {-0.0157541, -0.0187352, 0.0299132},
      {0.0767102, -0.0231764, 0.166465},
  },
  {
      {0.0154687, 0.0197088, 0.0059591},
      {-0.0542042, 0.00163079, 0.0979205},
      {-0.0448974, -0.00921851, 0.0404937},
      {-0.0663505, -0.0703455, 0.116126},
      {-0.0119812, -0.0690807, 0.0734746},
      {-0.303137, 0.198012, 0.0648043},
      {-0.00221325, -0.0118877, 0.00777958},
      {0.244127, 0.146886, 0.300241},
  },
  {
      {-0.0332222, 0.0289658, 0.00666637},
      {-0.0188828, -0.0242137, 0.0236653},
      {-0.0302107, -0.046473, 0.0444777},
      {-0.02292, -0.074628, 0.146228},
      {-0.116337, 0.137171, 0.0851569},
      {-0.103116, -0.0501278, 0.0472798},
      {0.0028868, 0.479385, 0.195481},
      {0.0753066, 0.0146559, 0.0448822},
  },
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
*/

#define GET_NOISE_COORD \
vec2 texDim = textureSize(u_depthTexture, 0).xy; \
vec2 texelSize = 1.0 / texDim; \
ivec2 noiseDim = textureSize(u_ssaoNoiseTexture, 0); \
const vec2 noiseCoord = vec2(float(texDim.x) / float(noiseDim.x), float(texDim.y) / (noiseDim.y)) * (posToTexCoord(pos) + vec2(u_vp.frameIndex * texelSize.x, u_vp.frameIndex * texelSize.y));

float ssao()  {
    vec2 pos = v_texCoord * 2.0 - 1.0;

    float depth = texture(u_depthTexture, posToTexCoord(pos)).r;
	vec3 fragPos = matrixMultiplyProjection(u_vp.invViewProj, vec3(pos, depth));
    vec3 normal = texelFetch(u_normalRoughnessTexture, ivec2(gl_FragCoord.xy), 0).xyz;

	GET_NOISE_COORD;

    vec3 randomVec = vec3(texture(u_ssaoNoiseTexture, noiseCoord).xy, 0.0);
	
	//Create TBN matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	const float bias = 0.04;

	for (int i = 0; i < SSAO_KERNEL_SIZE; i++) {
		vec3 samplePos = TBN * SSAO_KERNEL[u_vp.frameIndex % SSAO_SAMPLE_SET_COUNT][i].xyz; 
		samplePos = fragPos + samplePos * SSAO_RADIUS;
		
		// project
        vec4 viewSamplePos = u_vp.view * vec4(samplePos, 1.0);
		vec2 offset = matrixMultiplyProjection(u_vp.projection, viewSamplePos.xyz).xy;
		
		float sampleDepth = texture(u_depthTexture, posToTexCoord(offset)).r;
        float sampleLinearDepth = /*linearizeDepth(sampleDepth, 0.01, 1000.0);*/(u_vp.view * vec4(matrixMultiplyProjection(u_vp.invViewProj, vec3(offset, sampleDepth)), 1.0)).z;
        //FragColor = sampleDepth;

		float rangeCheck = smoothstep(0.0, 1.0, SSAO_RADIUS / abs(sampleLinearDepth - viewSamplePos.z));
		occlusion += (viewSamplePos.z <= sampleLinearDepth - bias ? rangeCheck : 0.0);
        //if (sampleDepth < texture(u_positionDepth, v_texCoord).w)
        //    occlusion += 1.0;
	}
    
	return 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));
}

/*
float saturate(float a) {
	return min(max(a, 0), 1);
}
*/

float length2(vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float computeAO(vec3 normal, vec2 direction, vec2 texelSize, vec3 fragPos) {
    vec2 pos = v_texCoord * 2.0 - 1.0;

	//float RAD = 1.0;
	//float RAD_FOR_DIRECTION = length(direction * vec2(10.0) / (vec2(abs(fragPos.z)) * screenSize));

	vec3 viewVector = normalize(fragPos);

	vec3 leftDirection = cross(viewVector, vec3(direction, 0));
	//vec3 projectedNormal = normal - dot(leftDirection, normal) * leftDirection;
	//projectedNormal = normalize(projectedNormal);

	vec3 tangent = cross(/*projectedNormal*/normal, leftDirection);

	const float bias = (PI / 360.0) * 10.0;

	float tangentAngle = atan(tangent.z / length(tangent.xy));
	float sinTangentAngle = sin(tangentAngle + bias);

	//float highestZ = -INFINITY;
	//vec3 foundPos = vec3(0, 0, -INFINITY);
    float horizonAngle = tangentAngle + bias;
    vec3 horizonVector;// = vec3(0.0);
    
    vec2 marchPosition = pos;
	for (int i = 1; i < 4; i++) {
		marchPosition += i * texelSize * 2.0 * direction * 16.0/* * (1.0 + SSAO_KERNEL[u_vp.frameIndex % SSAO_SAMPLE_SET_COUNT][i].x * 0.5)*/ / fragPos.z;
		
		float depth = texture(u_depthTexture, posToTexCoord(marchPosition)).x;
        //if (depth == 1.0) continue;
        vec3 fragPosMarch = (u_vp.view * vec4(matrixMultiplyProjection(u_vp.invViewProj, vec3(marchPosition, depth)), 1.0)).xyz;
		
		vec3 crntVector = fragPosMarch - fragPos;

		//float rangeCheck = 1 - saturate(length(fragPosMarch - fragPos) / RAD-1);

		if (length2(crntVector) < HBAO_RADIUS * HBAO_RADIUS) {
			horizonAngle = max(horizonAngle, atan(crntVector.z / length(crntVector.xy)));
            horizonVector = crntVector;
		}
	}
    //horizonAngle = horizonAngle % 6.3;
    //horizonAngle += 2.0;

	float sinHorizonAngle = sin(horizonAngle);

	//vec2 rez = vec2(saturate((sinHorizonAngle - sinTangentAngle)) / 2, projectedLen);

    float norm = length(horizonVector) / HBAO_RADIUS;
    float attenuation = 1.0 - norm * norm;

    //if (horizonVector.x == 0.0) return 0.0;

	return attenuation * (sinHorizonAngle - sinTangentAngle);
}

float hbao() {
    vec2 pos = v_texCoord * 2.0 - 1.0;

    GET_NOISE_COORD;

    float randomAngle = texture(u_ssaoNoiseTexture, noiseCoord).x + (u_vp.frameIndex % 16) / 16.0;
    vec3 randomVec = vec3(sin(randomAngle * 360.0), cos(randomAngle * 360.0), 0.0);

    float depth = texture(u_depthTexture, posToTexCoord(pos)).x;

	vec3 fragPos = (u_vp.view * vec4(matrixMultiplyProjection(u_vp.invViewProj, vec3(pos, depth)), 1.0)).xyz;

    vec3 normal = texelFetch(u_normalRoughnessTexture, ivec2(gl_FragCoord.xy), 0).xyz;

	float rez = 0.0;

	rez += computeAO(normal, vec2(randomVec), texelSize, fragPos);
	rez += computeAO(normal, -vec2(randomVec), texelSize, fragPos);
	rez += computeAO(normal, vec2(-randomVec.y, -randomVec.x), texelSize, fragPos);
	rez += computeAO(normal, vec2(randomVec.y, randomVec.x), texelSize, fragPos);

    return 1.0 - rez * 0.25;
}

void main() {
    FragColor = ssao();
}
