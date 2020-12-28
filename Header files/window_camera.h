#ifndef WINDOW_CAMERA_H
#define WINDOW_CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include<iostream>
#include <vector>
#include <string>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

extern bool first_mouse;
extern double lastX;
extern double lastY;

const unsigned int WINDOW_WIDTH = 1000;
const unsigned int WINDOW_HEIGHT = 800;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

struct CameraState {
    float Yaw = YAW;
    float Pitch = PITCH;
    float Zoom = ZOOM;
    float MouseSensitivity = SENSITIVITY;
    float MovementSpeed = SPEED;

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void UpdateCameraVectors();
};

extern CameraState global_camera_state;
extern float height_scale;

extern bool blinn_phong;
extern bool blinnKeyPressed;

extern bool post_effect;
extern bool postEffectKeyPressed;

extern bool normal_mapping_effect;
extern bool normalMappingKeyPressed;

extern bool activate_rotation;
extern bool activaleRotationKeyPressed;

extern bool parallax;
extern bool parallaxKeyPressed;

extern bool CookTorrance;
extern bool CookTorranceKeyPressed;

void InitializeAndConfigurateGLFW();
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);


class WindowCamera {
public:
    GLFWwindow* glfw_window;

public:
    WindowCamera();

    glm::mat4 GetViewMatrix();

    void InitializeCameraVectorial(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    void InitializeCameraNumerically(float posX, float posY, float posZ,
        float upX, float upY, float upZ, float yaw, float pitch);

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

    int InitializeWindow(int window_width, int window_heigh, std::string window_name);
    bool IsOpen();
    void ProcessInput(float delta_time);
    void SwapBuffersAndPollEvents();
};
#endif
