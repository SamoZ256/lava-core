#version 450

layout (location = 0) out vec4 FragColor;
//layout (location = 1, color_attachment_index = 3) out vec2 gbufferMotion;

layout (location = 0) in vec3 v_position;
//layout (location = 1) in vec4 v_crntPosition;
//layout (location = 2) in vec4 v_prevPosition;

layout (set = 0, binding = 0) uniform samplerCube u_skylightTexture;

void main() {
    /*
    FragColor = vec4(atmosphere(
        normalize(v_position),  // normalized ray direction
        vec3(0, 6372e3, 0),             // ray origin
        u_vp.lightPos,                  // position of the sun
        22.0,                           // intensity of the sun
        6371e3,                         // radius of the planet in meters
        6471e3,                         // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
        8e3,                            // Rayleigh scale height
        1.2e3,                          // Mie scale height
        0.758                           // Mie preferred scattering direction
    ), 1.0);
    */
    FragColor = vec4(texture(u_skylightTexture, v_position).rgb, 0.0);

    //gbufferMotion = (v_prevPosition.xy / v_prevPosition.w - v_crntPosition.xy / v_crntPosition.w);
}
