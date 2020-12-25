#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "../Header files/stb_image.h"

#include "../Header files/shader.h"
#include "../Header files/window.h"
#include "../Header files/model.h"
#include "../Header files/texture.h"
#include "../Header files/window_camera.h"


void LoadPointerForOpenGLFunctions() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error in loading OpenGL functions" << std::endl;
    }
}

float delta_time = 0.0f;
float last_frame = 0.0f;

bool first_mouse = true;
double lastX;
double lastY;

CameraState global_camera_state;

// ���������
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Blinn (false) or Blinn-Phong (true)
bool blinn_phong = true;
bool blinnKeyPressed = false;


int main()
{
    WindowCamera window_camera;
    window_camera.InitializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Window name");
    window_camera.InitializeCameraVectorial(glm::vec3(0.0f, 0.0f, 3.0f));

    LoadPointerForOpenGLFunctions();

    ShaderProgram cube_shader_program;
    ShaderProgram lamp_shader_program;
    ShaderProgram plane_shader_program;
    cube_shader_program.Build("Shaders/CubeVertexShader", "Shaders/CubeFragmentShader");
    lamp_shader_program.Build("Shaders/LampVertexShader", "Shaders/LampFragmentShader");
    plane_shader_program.Build("Shaders/PlaneVertexShader", "Shaders/PlaneFragmentShader");

    Texture wood_texture;
    wood_texture.Inizialize("Textures/wood.png");

    float cube_verteces[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    float plane_vertices[] = {
        // ����������            // �������         // ���������� ����������
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    // 1. ����������� VAO (� VBO) ���������
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // 2. ����������� VAO (� VBO) ����
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verteces), cube_verteces, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

    // ������������ ��������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // �������� ��������
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 2. ����������� VAO (� VBO) ��������� ����
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    // �������� ��������, ��� �� ��������� ��� �������� ��������� �����, ����� �������� ����������� ������ ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);

    while (window_camera.IsOpen()) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        window_camera.ProcessInput(delta_time);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        wood_texture.Bind(GL_TEXTURE_2D);

        // 1) ��������� ����
        cube_shader_program.Use();
        cube_shader_program.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        cube_shader_program.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        cube_shader_program.setVec3("lightPos", lightPos);
        cube_shader_program.setVec3("viewPos", global_camera_state.Position);

        glm::mat4 cube_projection = glm::perspective(glm::radians(global_camera_state.Zoom),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 cube_view = window_camera.GetViewMatrix();
        cube_shader_program.setMat4("projection", cube_projection);
        cube_shader_program.setMat4("view", cube_view);

        // ������� ��������������
        glm::mat4 cube_model = glm::mat4(1.0f);
        cube_shader_program.setMat4("model", cube_model);

        // ��������� ����
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // 2) ��������� ��������� ����
        lamp_shader_program.Use();
        lamp_shader_program.setMat4("projection", cube_projection);
        lamp_shader_program.setMat4("view", cube_view);
        glm::mat4 light_cube_model = glm::mat4(1.0f);
        light_cube_model = glm::translate(light_cube_model, lightPos);
        light_cube_model = glm::scale(light_cube_model, glm::vec3(0.2f)); // ���, �������� �������
        lamp_shader_program.setMat4("model", light_cube_model);

        // ��������� ��������� ����
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 3) ��������� ���������
        plane_shader_program.Use();
        glm::mat4 plane_projection = glm::perspective(glm::radians(global_camera_state.Zoom),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 plane_view = window_camera.GetViewMatrix();
        plane_shader_program.setMat4("projection", plane_projection);
        plane_shader_program.setMat4("view", plane_view);

        // ��������� uniforms-���������� ���������
        plane_shader_program.setVec3("viewPos", global_camera_state.Position);
        plane_shader_program.setVec3("lightPos", lightPos);
        plane_shader_program.setInt("blinn_phong", blinn_phong);

        // ��������� ����
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        window_camera.SwapBuffersAndPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwTerminate();

    return 0;
}
