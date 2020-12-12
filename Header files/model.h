#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<iostream>
#include <string>


class Model {
public:
    float* vertices;
    unsigned int VBO, VAO;

public:
    Model();

    void Initialize(float init_vertices[]);
};

#endif