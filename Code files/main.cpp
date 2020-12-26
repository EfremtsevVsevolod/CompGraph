#include <iostream>
#include <map>
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
glm::vec3 lightPos(1.5f, 1.5f, 1.0f);

// Blinn (false) or Blinn-Phong (true)
bool blinn_phong = false;
bool blinnKeyPressed = false;

bool post_effect = false;
bool postEffectKeyPressed = false;

// INITIALIZATIONS
void InitCube(unsigned& cubeVAO, unsigned& cubeVBO) {
    float cube_verteces[] = {
        // координаты         // нормали           // текстурные координаты
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // настраиваем VAO (и VBO) куба
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verteces), cube_verteces, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

    // координатные атрибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // атрибуты нормалей
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // текстурные атрибуты
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}

void InitPlane(unsigned& planeVAO, unsigned& planeVBO) {
    float plane_vertices[] = {
        // координаты            // нормали         // текстурные координаты
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    // 1) настраиваем VAO (и VBO) плоскости
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
}

void InitQuad(unsigned& quadVAO, unsigned& quadVBO) {
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

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

// POST-EFFECT
void AddPostEffect(unsigned& framebuffer, unsigned& textureColorbuffer, unsigned& RBO) {
    // 1) конфигурация фреймбуфера
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 2) текстура в которую будем производить рендеринг
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // 3) создание объекта рендербуфера дла прикрепляемых объектов глубины и трафарета
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
}

// PREPARATIONS
void PrepareCube(WindowCamera& window_camera, ShaderProgram& cube_shader_program) {
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
    cube_model = glm::translate(cube_model, glm::vec3(0.0f, 0.5f, 0.0f));
    cube_shader_program.setMat4("model", cube_model);
}

void PrepareLightCube(WindowCamera& window_camera, ShaderProgram& lamp_shader_program) {
    lamp_shader_program.Use();
    lamp_shader_program.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lamp_shader_program.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    lamp_shader_program.setVec3("lightPos", lightPos);
    lamp_shader_program.setVec3("viewPos", global_camera_state.Position);

    glm::mat4 light_cube_projection = glm::perspective(glm::radians(global_camera_state.Zoom),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 light_cube_view = window_camera.GetViewMatrix();
    lamp_shader_program.setMat4("projection", light_cube_projection);
    lamp_shader_program.setMat4("view", light_cube_view);

    glm::mat4 light_cube_model = glm::mat4(1.0f);
    light_cube_model = glm::translate(light_cube_model, lightPos);
    light_cube_model = glm::scale(light_cube_model, glm::vec3(0.2f)); // куб, меньшего размера
    lamp_shader_program.setMat4("model", light_cube_model);
}

void PreparePlane(WindowCamera& window_camera, ShaderProgram& plane_shader_program) {
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

    // мировое преобразование
    glm::mat4 plane_model = glm::mat4(1.0f);
    plane_shader_program.setMat4("model", plane_model);
}

void PrepareQuad(ShaderProgram quad_shader_program) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    quad_shader_program.Use();
    quad_shader_program.setInt("post_effect", post_effect);
}

// RENDERINGS
void RenderCube(unsigned& cubeVAO, unsigned& cubeVBO, Texture& texture) {
    if (cubeVAO == 0) {
        InitCube(cubeVAO, cubeVBO);
    }

    texture.Bind(GL_TEXTURE_2D);
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);
}

void RenderLightCube(unsigned& lightVAO, unsigned& lightVBO) {
    if (lightVAO == 0) {
        InitCube(lightVAO, lightVBO);
    }

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);
}

void RenderPlane(unsigned& planeVAO, unsigned& planeVBO, Texture& texture) {
    if (planeVAO == 0) {
        InitPlane(planeVAO, planeVBO);
    }

    texture.Bind(GL_TEXTURE_2D);
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderQuad(unsigned& quadVAO, unsigned& quadVBO,
                unsigned& framebuffer, unsigned& textureColorbuffer, unsigned& RBO) {
    if (quadVAO == 0) {
        InitQuad(quadVAO, quadVBO);
        AddPostEffect(framebuffer, textureColorbuffer, RBO);
    }

    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderScene(WindowCamera& window_camera, std::map<std::string, ShaderProgram> shader_programs,
                 std::map<std::string, unsigned>& VAOs, std::map<std::string, unsigned>& VBOs,
                 std::map<std::string, Texture>& textures,
                 unsigned& framebuffer, unsigned& textureColorbuffer, unsigned& RBO) {
    // привязываем фреймбуфер и отрисовываем сцену в текстуру 
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PrepareCube(window_camera, shader_programs["main"]);
    RenderCube(VAOs["cube"], VBOs["cube"], textures["brickwall"]);

    PreparePlane(window_camera, shader_programs["main"]);
    RenderPlane(VAOs["plane"], VBOs["plane"], textures["plane"]);

    PrepareLightCube(window_camera, shader_programs["lamp"]);
    RenderLightCube(VAOs["lamp"], VBOs["lamp"]);

    PrepareQuad(shader_programs["quad"]);
    RenderQuad(VAOs["quad"], VBOs["quad"], framebuffer, textureColorbuffer, RBO);
}


int main()
{
    WindowCamera window_camera;
    window_camera.InitializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Window name");
    window_camera.InitializeCameraVectorial(glm::vec3(0.0f, 0.0f, 3.0f));

    LoadPointerForOpenGLFunctions();

    ShaderProgram main_shader_program;
    ShaderProgram lamp_shader_program;
    ShaderProgram quad_shader_program;
    //ShaderProgram simple_depth_shader_program;

    main_shader_program.Build("Shaders/MainVertexShader", "Shaders/MainFragmentShader");
    lamp_shader_program.Build("Shaders/LampVertexShader", "Shaders/LampFragmentShader");
    quad_shader_program.Build("Shaders/ScreenVertexShader", "Shaders/ScreenFragmentShader");
    //simple_depth_shader_program.Build("Shaders/SimpleDepthVertexShader", "Shaders/SimpleDepthFragmentShader");

    quad_shader_program.Use();
    quad_shader_program.setInt("screenTexture", 0);

    std::map<std::string, ShaderProgram> shader_programs;
    shader_programs.insert({ "main", main_shader_program });
    shader_programs.insert({ "lamp", lamp_shader_program });
    shader_programs.insert({ "quad", quad_shader_program });
    //shader_programs.insert({ "simple_depth", simple_depth_shader_program });

    glEnable(GL_DEPTH_TEST);

    unsigned cubeVBO = 0, cubeVAO = 0;
    unsigned planeVBO = 0, planeVAO = 0;
    unsigned int quadVAO = 0, quadVBO = 0;

    std::map<std::string, unsigned> VAOs, VBOs;
    VAOs.insert({ "cube", 0 });
    VAOs.insert({ "plane", 0 });
    VBOs.insert({ "lamp", 0 });
    VBOs.insert({ "quad", 0 });

    // для постэффекта
    unsigned int framebuffer = 0;
    unsigned int textureColorbuffer = 0;
    unsigned int RBO = 0;

    Texture wood_texture;
    Texture brickwall_texture;

    wood_texture.Inizialize("Textures/wood.png");
    brickwall_texture.Inizialize("Textures/brickwall.jpeg");

    std::map<std::string, Texture> textures;
    textures.insert({ "plane", wood_texture });
    textures.insert({ "brickwall", brickwall_texture });


    while (window_camera.IsOpen()) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        window_camera.ProcessInput(delta_time);

        RenderScene(window_camera, shader_programs, VAOs, VBOs, textures,
                    framebuffer, textureColorbuffer, RBO);

        window_camera.SwapBuffersAndPollEvents();
    }

    // освобождение занятых ресурсов
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();

    return 0;
}
