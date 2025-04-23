#version 330 core

out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 tex_coords;

uniform sampler2D sprite_texture;
uniform vec2 uv0;
uniform vec2 uv1;

void main() {
    vec2 interp_tex_coords = mix(uv0, uv1, tex_coords);
    vec4 tex_color = texture(sprite_texture, interp_tex_coords);

    frag_color = tex_color;
}
