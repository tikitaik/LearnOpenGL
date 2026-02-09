#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {

public: 

    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;

    float yaw;
    float pitch;
    float fov;

    float lastX;
    float lastY;

    bool firstMouse;

    const float CAMERASPEED = 2.5f;

    Camera(glm::vec3 posIn, glm::vec3 frontIn, glm::vec3 upIn, int screenWidth, int screenHeight) {

        // camera vars
        pos   = posIn;
        front = frontIn;
        up    = upIn;
        /*
        std::cout << "pos: " << pos.x << ", " << pos.y << ", " << pos.z << '\n';
        std::cout << "front: " << front.x << ", " << front.y << ", " << front.z << '\n';
        std::cout << "up: " << up.x << ", " << up.y << ", " << up.z << '\n';
        */
        yaw   = -90.0f;
        pitch = 0.0f;
        fov   = 45.0f;

        lastX = (float)screenWidth / 2;
        lastY = (float)screenHeight / 2;

        firstMouse = true;
        
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouse(double xpos, double ypos);
    void ProcessScroll(double yoffset);
    glm::mat4 GetViewMatrix();
};

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {

    if (direction == FORWARD) {
        pos += front * CAMERASPEED * deltaTime;
    }
    if (direction == BACKWARD) {
        pos -= front * CAMERASPEED * deltaTime;
    }
    if (direction == LEFT) {
        pos -= glm::normalize(glm::cross(front, up)) * CAMERASPEED * deltaTime;
    }
    if (direction == RIGHT) {
        pos += glm::normalize(glm::cross(front, up)) * CAMERASPEED * deltaTime;
    }
    if (direction == UP) {
        pos += up * CAMERASPEED * deltaTime;
    }
    if (direction == DOWN) {
        pos -= up * CAMERASPEED * deltaTime;
    }
}

void Camera::ProcessMouse(double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = xpos;
    lastY = ypos;

    const float sens = 0.1f;
    xoffset *= sens;
    yoffset *= sens;

    yaw   += xoffset;
    pitch += yoffset;
    
    pitch = pitch > 89.0f ? 89.0 : pitch;
    pitch = pitch < -89.0f ? -89.0 : pitch;

    //std::cout << yaw << ' ' << pitch << '\n';

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}


void Camera::ProcessScroll(double yoffset) {
    
    fov -= (float)yoffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    } else if (fov > 45.0f) {
        fov = 45.0f;
    }
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(pos, pos + front, up);
}
