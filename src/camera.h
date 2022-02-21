#ifndef PONS_CAMERA_H
#define PONS_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "utils.h"

enum ECameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

const float DEFAULT_YAW          = -90.0f;
const float DEFAULT_PITCH        =  0.0f;
const float DEFAULT_SPEED        =  2.5f;
const float DEFAULT_SENSITIVITY  =  0.1f;
const float DEFAULT_ZOOM         =  45.0f;
const glm::vec3 DEFAULT_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f); // FIXME: this is potentially dangerous, don't set const from vec

class Camera {
public: // methods
    explicit Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = DEFAULT_WORLD_UP, float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(ECameraMovement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    inline glm::vec3 GetPosition() {
        return Position;
    }
    inline glm::vec3 GetFrontVector() {
        return Front;
    }
    inline glm::vec3 GetRightVector() {
        return Right;
    }
    inline glm::vec3 GetUpVector() {
        return Up;
    }
    inline float GetYaw() {
        return Yaw;
    }
    inline float GetPitch() {
        return Pitch;
    }
    inline float GetMovementSpeed() {
        return MovementSpeed;
    }
    inline float GetMouseSensitivity() {
        return MouseSensitivity;
    }
    inline float GetZoom() {
        return Zoom;
    }

private: // methods
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

private: // properties
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
};


#endif //PONS_CAMERA_H
