#version 330 core
layout(location = 0) in vec3 aPos;

struct FigurePosition {
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform FigurePosition figure_position;
uniform mat4 light_space;


void main() {
    gl_Position = light_space * figure_position.model * vec4(aPos, 1.0);
}