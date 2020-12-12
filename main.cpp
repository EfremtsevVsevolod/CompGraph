#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <string>

const unsigned int WINDOW_WIDTH = 1000;
const unsigned int WINDOW_HEIGHT = 800;

void InitializeAndConfigurateGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

class Window {
public:
    GLFWwindow* glfw_window;

public:
    Window() = default;

    int Initialize(int window_width, int window_heigh, std::string window_name) {
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

    bool IsOpen() {
        return !glfwWindowShouldClose(glfw_window);
    }

    void ProcessInput() {
        if (glfwGetKey(glfw_window, GLFW_KEY_C) == GLFW_PRESS) {
            glfwSetWindowShouldClose(glfw_window, true);
        }
    }

    void SwapBuffersAndPollEvents() {
        glfwSwapBuffers(glfw_window);
        glfwPollEvents();
    }
};

int LoadPointerForOpenGLFunctions() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error in loading OpenGL functions" << std::endl;
        return -1;
    }

    return 0;
}


int main()
{
    InitializeAndConfigurateGLFW();

    Window window;
    window.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Window name");

    LoadPointerForOpenGLFunctions();

    ShaderProgram shader_program;
    shader_program.Build("VertexShader", "FragmentShader");

    // ������� ������ (� ������(��)) � ��������� ��������� ���������
    // ------------------------------------------------------------------
    float vertices[] = {
        // ����������         // �����
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // ������ ������ �������
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // ������ ����� �������
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // ������� �������
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // ������� ��������� ������ ���������� �������, ����� ��������� � ������������� ��������� �����(�), � ����� ������������� ��������� �������(�).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ������������ ��������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // �������� ��������
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ����� ����� �� ������ �������� �������� V��, ����� ������ ������ V�� �������� �� �������� ���� VAO, �� ��� ����� ����������.
    // ��������� ������ �������� VAO ������� ������ glBindVertexArray � ����� ������, ������� �� ������ �� ������� �������� VAO (��� VBO), ����� ��� ��������������� �� ���������.
    // glBindVertexArray(0);

    while (window.IsOpen()) {
        window.ProcessInput();

        // ���������
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ��������� ������������
        shader_program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.SwapBuffersAndPollEvents();
    }

    // glfw: ����� ���������� ��������� � ������� �������. ����� ������� �����\������� (���� �� ������/�������� ������, ��������� ������ ���� � �.�.)
   // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // �����������: ����������� ��� �������, ��� ������ ��� ������ ��� ����������:
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
