#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;

layout (push_constant) uniform VP {
    mat4 invViewProj;
    vec3 viewPos;
} u_vp;

layout (set = 0, binding = 0, input_attachment_index = 0, color_attachment_index = 1) uniform subpassInput u_gbufferDiffuse;
layout (set = 0, binding = 1, input_attachment_index = 1, color_attachment_index = 2) uniform subpassInput u_gbufferNormal;
//Depth attachment can be used as input attachment only with the Vulkan backend. Therefore, we need to create a separate color attachment for the depth.
layout (set = 0, binding = 2, input_attachment_index = 2, color_attachment_index = 3) uniform subpassInput u_gbufferDepth;

const float ambient = 0.1;
const float specularStrangth = 0.5;

const vec3 lightColor = vec3(2.0, 2.0, 2.0);
const vec3 lightDirection = normalize(vec3(2.0, -4.0, 1.0));

vec3 phongShading(in vec3 diffuseColor, in vec3 normal, in vec3 position) {
    float diffuse = max(dot(normal, -lightDirection), 0.0);

    vec3 viewDir = normalize(u_vp.viewPos - position);
    vec3 reflectDir = reflect(lightDirection, normal);
    float specular = specularStrangth * pow(max(dot(viewDir, reflectDir), 0.0), 32);

    return (ambient + diffuse + specular) * lightColor * diffuseColor;
}

vec3 reconstructPositionFromDepth(in mat4 invViewProj, in vec2 texCoord, in float depth) {
    vec3 posInViewProj = vec3(texCoord * 2.0 - 1.0, depth);
    vec4 position = invViewProj * vec4(posInViewProj, 1.0);
    position.xyz /= position.w;

    return position.xyz;
}

void main() {
    vec3 diffuseColor = subpassLoad(u_gbufferDiffuse).rgb;
    vec3 normal = subpassLoad(u_gbufferNormal).xyz;
    float depth = subpassLoad(u_gbufferDepth).x;
    vec3 position = reconstructPositionFromDepth(u_vp.invViewProj, v_texCoord, depth);

    FragColor = vec4(phongShading(diffuseColor, normal, position), 1.0);
}
