#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;


struct FigurePosition {
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform FigurePosition figure_position;


void main() {
	gl_Position = figure_position.projection * figure_position.view * figure_position.model * vec4(aPos, 1.0);
}