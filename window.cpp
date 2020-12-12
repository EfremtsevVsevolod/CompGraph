#include "window.h"


void InitializeAndConfigurateGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window() = default;

int Window::Initialize(int window_width, int window_heigh, std::string window_name) {
    InitializeAndConfigurateGLFW();

    glfw_window = glfwCreateWindow(window_width, window_heigh, window_name.c_str(), NULL, NULL);

    if (glfw_window == NULL) {
        std::cout << "Error in GLFW window creation" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(glfw_window);
    glfwSetFramebufferSizeCallback(glfw_window, FramebufferSizeCallback);

    return 0;
}

bool Window::IsOpen() {
    return !glfwWindowShouldClose(glfw_window);
}

void Window::ProcessInput() {
    if (glfwGetKey(glfw_window, GLFW_KEY_C) == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfw_window, true);
    }
}

void Window::SwapBuffersAndPollEvents() {
    glfwSwapBuffers(glfw_window);
    glfwPollEvents();
}
