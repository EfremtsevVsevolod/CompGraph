#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

struct {
    // Camera default and constant params
    const GLfloat SPEED = 2;
    const GLfloat SENSITIVITY = 0.1;
    const GLfloat ZOOM = 45;
    const GLfloat ZOOM_MAX_LIMIT = 45;
    const GLfloat ZOOM_MIN_LIMIT = 1;

    const GLfloat YAW = 0;
    const GLfloat PITCH = 0;
    const GLfloat PITCH_LIMIT = 89;

    const glm::vec3 POSITION = glm::vec3(0, 0, 0);
    const glm::vec3 UP = glm::vec3(0, 1, 0);
    const glm::vec3 FRONT = glm::vec3(0, 0, -1);
} CameraParams;

enum Direction {
    // enum for encoding  movement commands from keyboards
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD
};


class Camera {
public:
    // Two constructors
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
        GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
        : zoom(CameraParams.ZOOM), yaw(yaw), pitch(pitch), position(glm::vec3(posX, posY, posZ)),
        world_up(glm::vec3(upX, upY, upZ)), front(CameraParams.FRONT) {
        UpdateCameraVectorParams();
    }

    Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0),
           GLfloat yaw = 0, GLfloat pitch = 0)
        : yaw(yaw), pitch(pitch), position(position), world_up(up),
          zoom(CameraParams.ZOOM), front(CameraParams.FRONT) {
        UpdateCameraVectorParams();
    }

    // Getters
    GLfloat GetZoom() const {
        return zoom;
    }

    glm::vec3 GetPosition() const {
        return position;
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    // Input processing methods
    void ProcessKeyboard(Direction direction, float deltaTime) {
        GLfloat velocity = CameraParams.SPEED * deltaTime;

        if (direction == LEFT) {
            position -= right * velocity;
        }
        else if (direction == RIGHT) {
            position += right * velocity;
        }
        else if (direction == FORWARD) {
            position += front * velocity;
        }
        else if (direction == BACKWARD) {
            position -= front * velocity;
        }
        else {
            std::cout << "Error! Camera::ProcessKeyboard\n\t" <<
                "Faild when try process keyboard input" << std::endl;
        }
    }

    void ProcessMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean flag_constrain_pitch = true) {
        auto x_scaled_offset = CameraParams.SENSITIVITY * x_offset;
        auto y_scaled_offset = CameraParams.SENSITIVITY * y_offset;

        yaw += x_scaled_offset;
        pitch += y_scaled_offset;

        if (flag_constrain_pitch) {
            pitch = std::min(pitch, CameraParams.PITCH_LIMIT);
            pitch = std::max(pitch, -CameraParams.PITCH_LIMIT);
        }

        UpdateCameraVectorParams();
    }

    void ProcessMouseScroll(GLfloat zoom_offset) {
        zoom -= zoom_offset;
        zoom = std::min(zoom, CameraParams.ZOOM_MAX_LIMIT);
        zoom = std::max(zoom, CameraParams.ZOOM_MIN_LIMIT);
    }

private:
    void UpdateCameraVectorParams() {
        glm::vec3 new_front(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

        front = glm::normalize(new_front);
        up = glm::normalize(glm::cross(right, front));
        right = glm::normalize(glm::cross(front, world_up));
    }

private:
    // Current camera vector params
    glm::vec3 position;
    glm::vec3 world_up;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // Current camera scalar params
    GLfloat zoom;
    GLfloat pitch;
    GLfloat yaw;
};
