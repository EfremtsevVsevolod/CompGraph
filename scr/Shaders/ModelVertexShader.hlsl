#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;


out FigureParam {
    vec3 FragPos;
    mat3 TBNMatrix;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} figure_param;


struct FigurePosition {
    mat4 model;
    mat4 view;
    mat4 projection;
};



uniform FigurePosition figure_position;
uniform mat4 light_space;


void main() {
    gl_Position = figure_position.projection * figure_position.view * figure_position.model * vec4(aPos, 1.0);

    figure_param.FragPos = vec3(figure_position.model * vec4(aPos, 1.0));

    vec3 N = normalize(vec3(figure_position.model * vec4(aNorm, 0.0)));
    vec3 T = normalize(vec3(figure_position.model * vec4(aTangent, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    figure_param.TBNMatrix = mat3(T, B, N);

    figure_param.Normal = aNorm;

    figure_param.TexCoords = aTexCoords;

    figure_param.FragPosLightSpace = light_space * vec4(figure_param.FragPos, 1.0);
}