#include "../Header files/window_camera.h"


void InitializeAndConfigurateGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    global_camera_state.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    global_camera_state.ProcessMouseScroll(yoffset);
}

int WindowCamera::InitializeWindow(int window_width, int window_heigh, std::string window_name) {
    InitializeAndConfigurateGLFW();
    glfw_window = glfwCreateWindow(window_width, window_heigh, window_name.c_str(), NULL, NULL);

    if (glfw_window == NULL) {
        std::cout << "Error in GLFW window creation" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(glfw_window);

    glfwSetFramebufferSizeCallback(glfw_window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(glfw_window, MouseCallback);
    glfwSetScrollCallback(glfw_window, ScrollCallback);

    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return 0;
}

bool WindowCamera::IsOpen() {
    return !glfwWindowShouldClose(glfw_window);
}

void WindowCamera::ProcessInput(float delta_time) {
    if (glfwGetKey(glfw_window, GLFW_KEY_C) == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfw_window, true);
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS) {
        ProcessKeyboard(FORWARD, delta_time);
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS) {
        ProcessKeyboard(BACKWARD, delta_time);
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS) {
        ProcessKeyboard(LEFT, delta_time);
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS) {
        ProcessKeyboard(RIGHT, delta_time);
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn_phong = !blinn_phong;
        blinnKeyPressed = true;
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_E) == GLFW_PRESS && !postEffectKeyPressed)
    {
        post_effect = !post_effect;
        postEffectKeyPressed = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        postEffectKeyPressed = false;
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_N) == GLFW_PRESS && !normalMappingKeyPressed)
    {
        normal_mapping_effect = !normal_mapping_effect;
        normalMappingKeyPressed = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_N) == GLFW_RELEASE)
    {
        normalMappingKeyPressed = false;
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_R) == GLFW_PRESS && !activaleRotationKeyPressed)
    {
        activate_rotation = !activate_rotation;
        activaleRotationKeyPressed = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_R) == GLFW_RELEASE)
    {
        activaleRotationKeyPressed = false;
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_P) == GLFW_PRESS && !parallaxKeyPressed)
    {
        parallax = !parallax;
        parallaxKeyPressed = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        parallaxKeyPressed = false;
    }

    if (glfwGetKey(glfw_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS && parallaxKeyPressed)
    {
        if (height_scale > 0.0f) {
            height_scale -= 0.0005f;
        } else {
            height_scale = 0.0f;
        }
    } else if (glfwGetKey(glfw_window, GLFW_KEY_KP_ADD) == GLFW_PRESS && parallaxKeyPressed) {
        if (height_scale < 1.0f) {
            height_scale += 0.0005f;
        } else {
            height_scale = 1.0f;
        }
    }
}

void WindowCamera::SwapBuffersAndPollEvents() {
    glfwSwapBuffers(glfw_window);
    glfwPollEvents();
}


WindowCamera::WindowCamera() = default;

void WindowCamera::InitializeCameraVectorial(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    global_camera_state.Front = glm::vec3(0.0f, 0.0f, -1.0f);
    global_camera_state.MovementSpeed = SPEED;
    global_camera_state.MouseSensitivity = SENSITIVITY;
    global_camera_state.Zoom = ZOOM;
    global_camera_state.Position = position;
    global_camera_state.WorldUp = up;
    global_camera_state.Yaw = yaw;
    global_camera_state.Pitch = pitch;

    global_camera_state.UpdateCameraVectors();
}

void WindowCamera::InitializeCameraNumerically(float posX, float posY, float posZ, float upX, float upY, float upZ,
               float yaw, float pitch) {
    global_camera_state.Front = glm::vec3(0.0f, 0.0f, -1.0f);
    global_camera_state.MovementSpeed = SPEED;
    global_camera_state.MouseSensitivity = SENSITIVITY;
    global_camera_state.Zoom = ZOOM;
    global_camera_state.Position = glm::vec3(posX, posY, posZ);
    global_camera_state.WorldUp = glm::vec3(upX, upY, upZ);
    global_camera_state.Yaw = yaw;
    global_camera_state.Pitch = pitch;
    global_camera_state.UpdateCameraVectors();
}

glm::mat4 WindowCamera::GetViewMatrix() {
    return glm::lookAt(global_camera_state.Position,
        global_camera_state.Position + global_camera_state.Front, global_camera_state.Up);
}

void WindowCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = global_camera_state.MovementSpeed * deltaTime;

    if (direction == FORWARD) {
        global_camera_state.Position += global_camera_state.Front * velocity;
    }

    if (direction == BACKWARD) {
        global_camera_state.Position -= global_camera_state.Front * velocity;
    }

    if (direction == LEFT) {
        global_camera_state.Position -= global_camera_state.Right * velocity;
    }

    if (direction == RIGHT) {
        global_camera_state.Position += global_camera_state.Right * velocity;
    }
}

void CameraState::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) {
            Pitch = 89.0f;
        }

        if (Pitch < -89.0f) {
            Pitch = -89.0f;
        }
    }

    UpdateCameraVectors();
}

void CameraState::ProcessMouseScroll(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 45.0f) {
        Zoom -= yoffset;
    }

    if (Zoom <= 1.0f) {
        Zoom = 1.0f;
    }

    if (Zoom >= 45.0f) {
        Zoom = 45.0f;
    }
}

void CameraState::UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    global_camera_state.Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
