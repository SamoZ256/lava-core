#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_position;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
    mat4 shadowViewProj;
    vec3 viewPos;
} u_vp;

layout (set = 0, binding = 1) uniform sampler2DShadow u_shadowTexture;

layout (set = 1, binding = 0) uniform sampler2D u_diffuseTexture;

const float ambient = 0.1;
const float specularStrangth = 0.5;

const vec3 lightColor = vec3(2.0, 2.0, 2.0);
const vec3 lightDirection = normalize(vec3(2.0, -4.0, 1.0));

vec3 phongShading(in vec3 diffuseColor) {
    float diffuse = max(dot(v_normal, -lightDirection), 0.0);

    vec3 viewDir = normalize(u_vp.viewPos - v_position);
    vec3 reflectDir = reflect(lightDirection, v_normal);
    float specular = specularStrangth * pow(max(dot(viewDir, reflectDir), 0.0), 32);

    return (ambient + diffuse + specular) * lightColor * diffuseColor;
}

float getVisibility() {
    vec4 shadowCoord = u_vp.shadowViewProj * vec4(v_position, 1.0);
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
    shadowCoord.y = 1.0 - shadowCoord.y;

    float bias = 0.05 * tan(acos(dot(v_normal, lightDirection)));
    bias = clamp(bias, 0.01, 0.02);

    return mix(1.0, texture(u_shadowTexture, vec3(shadowCoord.xy, shadowCoord.z - bias)), 0.7);
}

void main() {
    vec4 diffuseColor = texture(u_diffuseTexture, v_texCoord);
    if (diffuseColor.a < 0.5)
        discard;
    FragColor = vec4(phongShading(diffuseColor.rgb) * getVisibility(), 1.0);
}
