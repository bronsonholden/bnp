#version 330 core

out vec4 FragColor;

in vec2 TexCoord;


void main() {
    float dist = distance(TexCoord, vec2(0.5, 0.5));

    if (dist < 0.497 || dist > 0.500)
    {
        discard;
    }

    FragColor = vec4(vec3(0.2), 1);
}
