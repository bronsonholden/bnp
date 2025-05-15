#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 _aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    // quads are {{-0.5, -0.5}, {0.5, 0.5}} so scale it up to {{-1.0, -1.0}, {1.0, 1.0}}
    // to match clip space
    gl_Position = vec4(aPos.xy * 2, 0.0, 1.0);
    TexCoord = aTexCoord;
}
