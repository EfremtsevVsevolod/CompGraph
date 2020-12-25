#version 330 core
in vec4 FragPos;

uniform vec3 light_position;
uniform float far_plane;


void main() {
    float light_distance = length(FragPos.xyz - light_position);
    light_distance /= far_plane;
    gl_FragDepth = light_distance;
}
