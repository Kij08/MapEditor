//
// Created by Kij on 25/07/09.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/ext/quaternion_geometric.hpp>
#include "glm/vec3.hpp"

constexpr glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

class Camera {
    float yaw, pitch;
    float lastX, lastY;
    const float sensitivity = 15.f;

    glm::vec3 cameraForward = glm::vec3(1.0f, 1.0f, 1.0f);

public:
    explicit Camera(class Empty* o);

    //Function for updating the lastPos variables of the camera. This is used if camera movement is started/stopped to prevent jumps in the camera.
    void UpdateMousePos(double xPos, double yPos);

    //All in one solution for camera movement. Supply mouse position and update camera.
    void UpdateCamera(double xPos, double yPos);

    glm::vec3 GetCameraForward() { return cameraForward; };
    glm::vec3 GetCameraRight() { return glm::normalize(glm::cross(cameraForward, cameraUp)); };
    static glm::vec3 GetCameraUp() { return cameraUp; };
private:
    Empty* Owner;
};



#endif //CAMERA_H
