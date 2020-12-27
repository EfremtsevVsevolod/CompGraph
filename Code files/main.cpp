#include <iostream>
#include <map>
#include <string>
#include <utility>

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
void FreeResourcesAndTerminate(std::map<std::string, unsigned>& VAOs,
                               std::map<std::string, unsigned>& VBOs) {
    for (auto& VAO : VAOs) {
        glDeleteVertexArrays(1, &VAO.second);
    }

    for (auto& VBO : VBOs) {
        glDeleteVertexArrays(1, &VBO.second);
    }

    glfwTerminate();
}

float delta_time = 0.0f;
float last_frame = 0.0f;

void ProcessTime() {
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
}

bool first_mouse = true;
double lastX;
double lastY;

float rotation_angle;
float height_scale = 0.1;

CameraState global_camera_state;

// Освещение
glm::vec3 lightPos(1.5f, 1.5f, 1.0f);

// Blinn (false) or Blinn-Phong (true)
bool blinn_phong = false;
bool blinnKeyPressed = false;

bool post_effect = false;
bool postEffectKeyPressed = false;

bool normal_mapping_effect = true;
bool normalMappingKeyPressed = true;

bool activate_rotation = false;
bool activaleRotationKeyPressed = false;

bool parallax = true;
bool parallaxKeyPressed = true;


//  AUXILARY FUNCTIONS
std::pair<glm::vec3, glm::vec3>
CalculateTangentAndBitangentVectors(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
    glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec3 nm) {
    glm::vec3 tangent, bitangent;

    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    return std::make_pair(tangent, bitangent);
}

void InitializeTangentAndBitangentComponents(float vertices[], int vertices_size) {
    for (int i = 0; i < vertices_size; i += 14 * 3) {
        glm::vec3 pos1(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 pos2(vertices[14 + i], vertices[14 + i + 1], vertices[14 + i + 2]);
        glm::vec3 pos3(vertices[2 * 14 + i], vertices[2 * 14 + i + 1], vertices[2 * 14 + i + 2]);

        glm::vec3 nm(vertices[i + 3], vertices[i + 4], vertices[i + 5]);

        glm::vec2 uv1(vertices[i + 6], vertices[i + 7]);
        glm::vec2 uv2(vertices[14 + i + 6], vertices[14 + i + 7]);
        glm::vec2 uv3(vertices[2 * 14 + i + 6], vertices[2 * 14 + i + 7]);

        auto result = CalculateTangentAndBitangentVectors(pos1, pos2, pos3, uv1, uv2, uv3, nm);
        glm::vec3 tangent = result.first;
        glm::vec3 bitangent = result.second;

        for (int j = 0; j < 3; ++j) {
            vertices[j * 14 + i + 8] = tangent.x;
            vertices[j * 14 + i + 9] = tangent.y;
            vertices[j * 14 + i + 10] = tangent.z;
            vertices[j * 14 + i + 11] = bitangent.x;
            vertices[j * 14 + i + 12] = bitangent.y;
            vertices[j * 14 + i + 13] = bitangent.z;
        }
    }
}

// INITIALIZATIONS
void InitCube(unsigned& cubeVAO, unsigned& cubeVBO) {
    float cube_verteces[] = {
        // координаты         // нормали           // текст. к. // кас.   // бикасательные
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  0, 0, 0,  0, 0, 0,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  0, 0, 0,  0, 0, 0
    };
    int cube_verteces_size = sizeof(cube_verteces) / sizeof(float);

    InitializeTangentAndBitangentComponents(cube_verteces, cube_verteces_size);

    // настраиваем VAO (и VBO) куба
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verteces), cube_verteces, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

    // координатные атрибуты
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    
    // атрибуты нормалей
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // текстурные атрибуты
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));

    // касательные векторы
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));

    // бикасательные векторы
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InitPlane(unsigned& planeVAO, unsigned& planeVBO) {
    float plane_vertices[] = {
        // координаты            // нормали         // текст.к.   // T      // B
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,  0, 0, 0,  0, 0, 0,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,  0, 0, 0,  0, 0, 0,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,  0, 0, 0,  0, 0, 0,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,  0, 0, 0,  0, 0, 0,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,  0, 0, 0,  0, 0, 0,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f,  0, 0, 0,  0, 0, 0
    };

    int plane_vertices_size = sizeof(plane_vertices) / sizeof(float);

    InitializeTangentAndBitangentComponents(plane_vertices, plane_vertices_size);

    // настраиваем VAO (и VBO) плоскости
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

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
void PrepareCube(WindowCamera& window_camera, ShaderProgram& cube_shader_program,
                 glm::vec3 cube_position, float cube_size, bool enable_rotation) {
    cube_shader_program.Use();
    cube_shader_program.setVec3("lightPos", lightPos - cube_position);
    cube_shader_program.setVec3("viewPos", global_camera_state.Position - cube_position);

    cube_shader_program.setInt("diffuseMap", 0);
    cube_shader_program.setInt("normalMap", 1);
    cube_shader_program.setInt("depthMap", 2);

    cube_shader_program.setFloat("height_scale", height_scale);

    cube_shader_program.setBool("normal_mapping_flag", normal_mapping_effect);
    cube_shader_program.setBool("paralax_flag", parallax);

    glm::mat4 cube_projection = glm::perspective(glm::radians(global_camera_state.Zoom),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 cube_view = window_camera.GetViewMatrix();
    cube_shader_program.setMat4("projection", cube_projection);
    cube_shader_program.setMat4("view", cube_view);

    // мировое преобразование
    glm::mat4 cube_model = glm::mat4(1.0f);
    cube_model = glm::translate(cube_model, cube_position);
    cube_model = glm::scale(cube_model, glm::vec3(cube_size, cube_size, cube_size));

    if (enable_rotation && activate_rotation) {
        rotation_angle += glm::radians(delta_time * 30.0f);
    }
    if (enable_rotation) {
        cube_model = glm::rotate(cube_model, rotation_angle,
                                 glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
    }

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
    light_cube_model = glm::scale(light_cube_model, glm::vec3(0.2f));
    lamp_shader_program.setMat4("model", light_cube_model);
}

void PreparePlane(WindowCamera& window_camera, ShaderProgram& plane_shader_program) {
    plane_shader_program.Use();
    glm::mat4 plane_projection = glm::perspective(glm::radians(global_camera_state.Zoom),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 plane_view = window_camera.GetViewMatrix();
    plane_shader_program.setMat4("projection", plane_projection);
    plane_shader_program.setMat4("view", plane_view);

    plane_shader_program.setInt("diffuseMap", 0);
    plane_shader_program.setInt("normalMap", 1);

    plane_shader_program.setFloat("height_scale", 0);

    plane_shader_program.setBool("normal_mapping_flag", normal_mapping_effect);
    plane_shader_program.setBool("paralax_flag", false);

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
    quad_shader_program.setBool("post_effect", post_effect);
}

// RENDERINGS
void RenderCube(unsigned& cubeVAO, unsigned& cubeVBO, std::map<std::string, Texture>& textures) {
    if (cubeVAO == 0) {
        InitCube(cubeVAO, cubeVBO);
    }

    glBindVertexArray(cubeVAO);

    glActiveTexture(GL_TEXTURE0);
    textures["brickwall"].Bind(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    textures["brickwall_normals"].Bind(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE2);
    textures["brickwall_heights"].Bind(GL_TEXTURE_2D);

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

void RenderPlane(unsigned& planeVAO, unsigned& planeVBO, std::map<std::string, Texture>& textures) {
    if (planeVAO == 0) {
        InitPlane(planeVAO, planeVBO);
    }

    glBindVertexArray(planeVAO);

    glActiveTexture(GL_TEXTURE0);
    textures["plane"].Bind(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    textures["plane_normals"].Bind(GL_TEXTURE_2D);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderQuad(unsigned& quadVAO, unsigned& quadVBO,
                unsigned& framebuffer, unsigned& textureColorbuffer, unsigned& RBO) {
    if (quadVAO == 0) {
        InitQuad(quadVAO, quadVBO);
        AddPostEffect(framebuffer, textureColorbuffer, RBO);
    }

    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
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

    PrepareCube(window_camera, shader_programs["main"], glm::vec3(0.0f, 0.5f, 0.0f), 1.0f, true);
    RenderCube(VAOs["cube"], VBOs["cube"], textures);

    PrepareCube(window_camera, shader_programs["main"], glm::vec3(2.0f, 0.5f, 2.0f), 0.5f, false);
    RenderCube(VAOs["cube"], VBOs["cube"], textures);

    PreparePlane(window_camera, shader_programs["main"]);
    RenderPlane(VAOs["plane"], VBOs["plane"], textures);

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
    Texture wood_normals;
    Texture brickwall_texture;
    Texture brickwall_normals;
    Texture brickwall_heights;

    wood_texture.Inizialize("Textures/wood.png");
    wood_normals.Inizialize("Textures/NormalMapFloor.jpeg");
    brickwall_texture.Inizialize("Textures/bricks2.jpg");
    brickwall_normals.Inizialize("Textures/bricks2_normal.jpg");
    brickwall_heights.Inizialize("Textures/bricks2_disp.jpg");

    std::map<std::string, Texture> textures;
    textures.insert({ "plane", wood_texture });
    textures.insert({ "plane_normals", wood_normals });
    textures.insert({ "brickwall", brickwall_texture });
    textures.insert({ "brickwall_normals", brickwall_normals });
    textures.insert({ "brickwall_heights", brickwall_heights });

    while (window_camera.IsOpen()) {
        ProcessTime();

        window_camera.ProcessInput(delta_time);

        RenderScene(window_camera, shader_programs, VAOs, VBOs, textures,
                    framebuffer, textureColorbuffer, RBO);

        window_camera.SwapBuffersAndPollEvents();
    }

    FreeResourcesAndTerminate(VAOs, VBOs);

    return 0;
}
