#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_position;

layout (set = 0, binding = 0) uniform VP {
    mat4 viewProj;
    mat4 shadowProj;
    vec3 viewPos;
} u_vp;

layout (set = 0, binding = 1) uniform LIGHT {
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
} u_light;

layout (set = 0, binding = 2) uniform samplerCube/*Shadow*/ u_shadowTexture;

layout (set = 1, binding = 0) uniform sampler2D u_diffuseTexture;

const float ambient = 0.1;
const float specularStrangth = 0.5;

vec3 phongShading(in vec3 diffuseColor) {
    vec3 lightDir = v_position - u_light.position;
    float distance = length(lightDir);
    lightDir /= distance;
    float diffuse = max(dot(v_normal, -lightDir), 0.0);

    vec3 viewDir = normalize(u_vp.viewPos - v_position);
    vec3 reflectDir = reflect(lightDir, v_normal);
    float specular = specularStrangth * pow(max(dot(viewDir, reflectDir), 0.0), 32);

    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));   

    return (ambient + diffuse + specular) * attenuation * u_light.color * diffuseColor;
}

float getVisibility() {
    //float bias = 0.05;
    vec3 lightDir = v_position - u_light.position;
    float dist = length(lightDir);
    lightDir /= dist;
    //vec4 posInShadow = u_vp.shadowProj * vec4(0.0, 0.0, dist, 1.0);
    //float projectedDistance = posInShadow.z / posInShadow.w;
    lightDir.y = -lightDir.y;

    return float(texture(u_shadowTexture, lightDir).x/* > (dist / 10.0)*/);
    //return abs(/*(*/(0.01 * 10.0 / (10.0 + texture(u_shadowTexture, lightDir).x * (0.01 - 10.0)))/* > dist)*//* - dist*/);
    //return mix(1.0, texture(u_shadowTexture, vec4(lightDir, projectedDistance)), 0.7);
}

void main() {
    vec4 diffuseColor = texture(u_diffuseTexture, v_texCoord);
    if (diffuseColor.a < 0.5)
        discard;
    FragColor = vec4(phongShading(diffuseColor.rgb) * getVisibility(), 1.0);
}
