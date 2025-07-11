//
// Created by Kij on 25/06/18.
//

#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include "../Utils/InputManager.h"
#include "../Primitives/Empty.h"
#include "../Primitives/Camera.h"

enum class EViewerState {
    VIEWER_EDIT,
    VIEWER_PLACE,
    VIEWER_MOVE, //Move and NOSTATE should always be at the end of the enum
    VIEWER_NOSTATE
};

class SceneViewer : IInputResponse, public Empty {
public:
    explicit SceneViewer(Scene* s) : Empty(s, { .position = glm::vec3(20, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) }), ViewerState(EViewerState::VIEWER_EDIT), PreviousState(EViewerState::VIEWER_NOSTATE), camera(this) {};

    void SetState(EViewerState newState);

    void Begin() override;
    void Tick() override;

    Camera* GetCamera() { return &camera; };

private:
    void RespondToMouseButton(GLFWwindow* window, int button, int action, int mods) override;
    void RespondToKey(GLFWwindow *window, int key, int scancode, int action, int mods) override;
    void RespondToCursor(GLFWwindow* window, double xPos, double yPos) override;
    void RespondToScroll(GLFWwindow* window, double xOffset, double yOffset) override;

    void SetPreviousState(EViewerState newState) { PreviousState = newState; };


    EViewerState ViewerState;
    EViewerState PreviousState;

    Camera camera;

    //Movement values
    const float scrollStep = 5.f;
    const float moveSpeed = 300.f;
    const float scrollMoveDistance = 50.f;
    glm::vec3 moveVector = glm::vec3(0.0f, 0.0f, 0.0f);

    void AddMoveVector();
};



#endif //SCENEVIEWER_H
