#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// ���������
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: ������������� � ����������������
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // ����������������� ������� � ������ ������������� MacOS X
#endif

    // glfw �������� ����
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL for Ravesli.com", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: �������� ���� ���������� �� OpenGL-�������
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // �������������� ����� ��������� ���������
    // ------------------------------------
    Shader ourShader("VertexShader", "FragmentShader");  // ���� � ������ ��������

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


    // ���� ����������
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // ��������� �����
        // -----
        processInput(window);

        // ���������
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ��������� ������������
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
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

// ��������� ���� ������� �����: ������ GLFW � �������/���������� ������ ���� � ������ ����� � ��������������� ��������� ������ �������
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: ������ ���, ����� ���������� ������� ���� (������������� ��� ����. ��������), ���������� ������ �������
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // ����������, ��� ������� ������������� ����� �������� ����; �������� ��������,
    // ��� ������ � ������ ����� ����������� ������, ��� ������� �� retina -��������.
    glViewport(0, 0, width, height);
}
