#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

out vec3 frag_pos;
out vec4 frag_color;

void main() {
    frag_pos = vec3(model * vec4(aPos, 1.0));
    frag_color = color;
    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
