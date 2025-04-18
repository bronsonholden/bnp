#version 330 core

out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 tex_coords;

uniform sampler2D sprite_texture;
uniform vec2 uv0;
uniform vec2 uv1;

//const float adds[5] = float[](0.03f, 0.00f, -0.12f, -0.12f, -0.12f);
//float row = gl_FragCoord.y;
//tex_color += vec4(vec3(adds[idx]), tex_color.a);
//int idx = int(mod(floor(row), 5.0));

float random(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec2 interp_tex_coords = mix(uv0, uv1, tex_coords);
    vec4 tex_color = texture(sprite_texture, interp_tex_coords);

    vec2 cluster_size = vec2(128.0);
    vec2 frame_coords = interp_tex_coords - uv0;
    vec2 cluster_coord = floor(frame_coords * cluster_size) / cluster_size;

   // Create a random pattern based on texture coordinates and time
    float noise = random(cluster_coord * 10); // Adjust the scale and speed as needed

    // Add a subtle speckling effect based on the noise
    float intensity = 0.04; // Controls the amount of speckling

    frag_color = tex_color + vec4(vec3(noise * intensity), tex_color.a);
}
