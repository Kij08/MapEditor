//
// Created by Kij on 25/07/09.
//

#include "Camera.h"
#include <glm/trigonometric.hpp>
#include "Empty.h"
#include "../Scene/Scene.h"

Camera::Camera(Empty* o) : Owner(o) {

}

void Camera::UpdateMousePos(double xPos, double yPos) {
    float xOffset = -xPos + lastX;
    float yOffset = -yPos + lastY;
    lastX = xPos;
    lastY = yPos;
}

void Camera::UpdateCamera(double xPos, double yPos) {
    float xOffset = -xPos + lastX;
    float yOffset = -yPos + lastY;
    lastX = xPos;
    lastY = yPos;

    yaw += xOffset * sensitivity * Owner->GetLevel()->currentDeltaTime;
    pitch += yOffset * sensitivity * Owner->GetLevel()->currentDeltaTime;

    if(pitch > 89.0f) {
        pitch = 89.0f;
    }
    else if(pitch < -89.0f) {
        pitch = -89.0f;
    }

    cameraForward.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraForward.y = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraForward.z = glm::sin(glm::radians(pitch));
    cameraForward = glm::normalize(cameraForward);
}
