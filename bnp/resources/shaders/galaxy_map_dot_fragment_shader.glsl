#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

void main() {
    float snap_interval = 0.02f;
    //vec2 snapped_coord = floor(TexCoord / snap_interval) * snap_interval;
    vec2 snapped_coord = TexCoord;

    FragColor = vec4(1);
}
