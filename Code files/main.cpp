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

// Освещение
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Blinn (false) or Blinn-Phong (true)
bool blinn_phong = false;
bool blinnKeyPressed = false;

bool post_effect = false;
bool postEffectKeyPressed = false;


int main()
{
    WindowCamera window_camera;
    window_camera.InitializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Window name");
    window_camera.InitializeCameraVectorial(glm::vec3(0.0f, 0.0f, 3.0f));

    LoadPointerForOpenGLFunctions();

    ShaderProgram cube_shader_program;
    ShaderProgram lamp_shader_program;
    ShaderProgram plane_shader_program;
    ShaderProgram screen_shader_program;
    cube_shader_program.Build("Shaders/CubeVertexShader", "Shaders/CubeFragmentShader");
    lamp_shader_program.Build("Shaders/LampVertexShader", "Shaders/LampFragmentShader");
    plane_shader_program.Build("Shaders/PlaneVertexShader", "Shaders/PlaneFragmentShader");
    screen_shader_program.Build("Shaders/ScreenVertexShader", "Shaders/ScreenFragmentShader");

    screen_shader_program.Use();
    screen_shader_program.setInt("screenTexture", 0);

    Texture wood_texture;
    wood_texture.Inizialize("Textures/wood.png");

    glEnable(GL_DEPTH_TEST);

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
        // координаты            // нормали         // текстурные координаты
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    float quadVertices[] = {
        // атрибуты вершин в нормализованных координатах устройства для прямоугольника,
        // который имеет размеры экрана 

        // координаты   // текстурные координаты
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // 1) настраиваем VAO (и VBO) плоскости
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

    // 2) настраиваем VAO (и VBO) куба
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verteces), cube_verteces, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

    // координатные атрибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // атрибуты нормалей
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 3) настраиваем VAO (и VBO) светового куба
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 4) Настраиваем VAO (и VBO) прямоугольника
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // A) конфигурация фреймбуфера
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // B) текстура в которую будем производить рендеринг
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    // C) создание объекта рендербуфера дла прикрепляемых объектов глубины и трафарета
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // теперь, когда мы создали фреймбуфер и прикрепили все необходимые объекты,
    // проверяет завершение формирование фреймбуфера
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error! Main::Framebuffer is not complete!" << std::endl;
    }

    // чтобы все операции рендеринга снова оказывали визуальное воздействие на главное окно приложения
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (window_camera.IsOpen()) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        window_camera.ProcessInput(delta_time);

        // привязываем фреймбуфер и отрисовываем сцену в текстуру 
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1) Отрисовка куба
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

        // мировое преобразование
        glm::mat4 cube_model = glm::mat4(1.0f);
        cube_shader_program.setMat4("model", cube_model);

        // рендеринг куба
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // 2) Отрисовка светового куба
        lamp_shader_program.Use();
        lamp_shader_program.setMat4("projection", cube_projection);
        lamp_shader_program.setMat4("view", cube_view);
        glm::mat4 light_cube_model = glm::mat4(1.0f);
        light_cube_model = glm::translate(light_cube_model, lightPos);
        light_cube_model = glm::scale(light_cube_model, glm::vec3(0.2f)); // куб, меньшего размера
        lamp_shader_program.setMat4("model", light_cube_model);

        // рендеринг светового куба
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 3) Отрисовка плоскости
        plane_shader_program.Use();
        glm::mat4 plane_projection = glm::perspective(glm::radians(global_camera_state.Zoom),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 plane_view = window_camera.GetViewMatrix();
        plane_shader_program.setMat4("projection", plane_projection);
        plane_shader_program.setMat4("view", plane_view);

        // установка uniforms-переменных освещения
        plane_shader_program.setVec3("viewPos", global_camera_state.Position);
        plane_shader_program.setVec3("lightPos", lightPos);
        plane_shader_program.setInt("blinn_phong", blinn_phong);

        // рендеринг плоскости
        wood_texture.Bind(GL_TEXTURE_2D);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 4) Отрисовка прямоугольника
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screen_shader_program.Use();
        screen_shader_program.setInt("post_effect", post_effect);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        window_camera.SwapBuffersAndPollEvents();
    }

    // освобождение занятых ресурсов
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &lightVAO);

    glfwTerminate();

    return 0;
}
