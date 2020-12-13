#ifndef WINDOW_H
#define WINDOW_H



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
