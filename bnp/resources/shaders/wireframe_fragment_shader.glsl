#version 330 core

out vec4 out_frag_color;
in vec3 frag_pos;
in vec4 frag_color;

void main() {
    out_frag_color = frag_color;
}
