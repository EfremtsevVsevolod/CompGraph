#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<iostream>
#include <string>


const unsigned int WINDOW_WIDTH = 1000;
const unsigned int WINDOW_HEIGHT = 800;

void InitializeAndConfigurateGLFW();
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

class Window {
public:
    GLFWwindow* glfw_window;

public:
    Window();

    int Initialize(int window_width, int window_heigh, std::string window_name);
    bool IsOpen();
    void ProcessInput();
    void SwapBuffersAndPollEvents();
};
#endif
