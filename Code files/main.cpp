#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Header files/shader.h"
#include "../Header files/window.h"
#include "../Header files/model.h"

#include <iostream>
#include <string>


int LoadPointerForOpenGLFunctions() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error in loading OpenGL functions" << std::endl;
        return -1;
    }

    return 0;
}


int main()
{
    Window window;
    window.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Window name");

    LoadPointerForOpenGLFunctions();

    ShaderProgram shader_program;
    shader_program.Build("Shaders/VertexShader", "Shaders/FragmentShader");

    float vertices[] = {
        // координаты         // цвета
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // нижн€€ права€ вершина
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // нижн€€ лева€ вершина
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // верхн€€ вершина
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // координатные артибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // цветовые атрибуты
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (window.IsOpen()) {
        window.ProcessInput();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.SwapBuffersAndPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}
