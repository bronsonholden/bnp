#version 330 core

out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D sprite_texture;
uniform vec2 uv0;
uniform vec2 uv1;

void main() {
    vec2 interp_tex_coords = mix(uv0, uv1, tex_coords);

    if (tex_coords.y > 0.85f) {
        frag_color = vec4(1);
    } else {
        frag_color = vec4(0.678, 0.847, 0.902, 0.75);
    }
}
