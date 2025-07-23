//
// Created by Kij on 25/06/18.
//

#include "SceneViewer.h"

#include <iostream>
#include <bits/ostream.tcc>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>

#include "Scene.h"
#include "../Utils/FileManager.h"

std::string to_string(EViewerState state) {
    switch (state) {
        case EViewerState::VIEWER_EDIT:
            return "Edit Mode";
        case EViewerState::VIEWER_PLACE:
            return "Place Mode";
        case EViewerState::VIEWER_MOVE:
            return "Move Mode";
        case EViewerState::VIEWER_NOSTATE:
            return "No state";
        default:
            return "Unknown";
    }
}

SceneViewer::SceneViewer(Scene *s) :
    Empty(s, { .position = glm::vec3(20, 0, 0), .rotation = glm::vec3(0, 0, 0), .scale = glm::vec3(0.5, 0.5, 0.5) }),
    ViewerState(EViewerState::VIEWER_EDIT), PreviousState(EViewerState::VIEWER_EDIT), camera(this) {
    FileManager::WriteStringToLog("Create Scene Viewer");
}

bool SceneViewer::SetState(EViewerState newState) {

    //Dont change state to move if ImGui wants the mouse
    if (newState == EViewerState::VIEWER_MOVE && ImGui::GetIO().WantCaptureMouse) {
        return false;
    }

    PreviousState = ViewerState;
    ViewerState = newState;

    //If we are still holding a move key when the state changes then clear the move vector
    if (PreviousState == EViewerState::VIEWER_MOVE) {
        moveVector = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    return true;
}

void SceneViewer::EnumerateState() {
    if (ViewerState < EViewerState::VIEWER_MOVE) {
        //If the state is not in movement then cycle states until we reach movement. Then we loop back to the beginning
        EViewerState nextState = static_cast<EViewerState>(static_cast<int>(ViewerState) + 1);
        if (nextState == EViewerState::VIEWER_MOVE) {
            //If the next state would be move then reset state to beginning state.
            SetState(static_cast<EViewerState>(0));
        }
        else {
            SetState(nextState);
        }
    }
}

void SceneViewer::RespondToMouseButton(GLFWwindow* window, int button, int action, int mods) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS) {
                //Set state to move and disable the cursor
                if (SetState(EViewerState::VIEWER_MOVE)) {
                    std::cout << "Move Mode" << std::endl;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
            }
            else if (action == GLFW_RELEASE) {
                //When moving is finished return to previous state and reset cursor lock
                if (SetState(PreviousState)) {
                    std::cout << "End Move Mode" << std::endl;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
        break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            if (action == GLFW_PRESS) {
                EnumerateState();
            }
        break;
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                if (ViewerState == EViewerState::VIEWER_EDIT) {
                    //Get object under cursor for edit
                }
                else if (ViewerState == EViewerState::VIEWER_PLACE) {
                    //Place selected object in world
                }
            }
        break;

        default:
        break;
    }

}

void SceneViewer::RespondToKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
    //Move viewer if in movement mode
    if (ViewerState == EViewerState::VIEWER_MOVE) {
        //Set move vector axis for corresponding key value
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_W:
                    moveVector.y = 1.f;
                    break;
                case GLFW_KEY_A:
                    moveVector.x = -1.f;
                    break;
                case GLFW_KEY_S:
                    moveVector.y = -1.f;
                    break;
                case GLFW_KEY_D:
                    moveVector.x = 1.f;
                    break;
                case GLFW_KEY_E:
                    moveVector.z = 1.f;
                    break;
                case GLFW_KEY_Q:
                    moveVector.z = -1.f;
                    break;
            }
        }
        else if (action == GLFW_RELEASE) {
            switch (key) {
                case GLFW_KEY_W:
                    moveVector.y = 0;
                    break;
                case GLFW_KEY_A:
                    moveVector.x = 0;
                    break;
                case GLFW_KEY_S:
                    moveVector.y = 0;
                    break;
                case GLFW_KEY_D:
                    moveVector.x = 0;
                    break;
                case GLFW_KEY_E:
                    moveVector.z = 0;
                    break;
                case GLFW_KEY_Q:
                    moveVector.z = 0;
                    break;
            }
        }
    }
    else if (ViewerState == EViewerState::VIEWER_EDIT) {
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_W:
                    break;
                case GLFW_KEY_E:
                    break;
                case GLFW_KEY_R:
                    break;
            }
        }
    }
}

void SceneViewer::RespondToCursor(GLFWwindow* window, double xPos, double yPos) {
    //Move camera if in movement mode
    if (ViewerState == EViewerState::VIEWER_MOVE) {
        camera.UpdateCamera(xPos, yPos);
    }
    else {
        camera.UpdateMousePos(xPos, yPos);
    }
}

void SceneViewer::RespondToScroll(GLFWwindow *window, double xOffset, double yOffset) {
    //Moving forward or backwards in viewport based on scroll input
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (yOffset > 0) {
            objTransform.position += camera.GetCameraForward() * scrollMoveDistance;
        }
        else if (yOffset < 0) {
            objTransform.position -= camera.GetCameraForward() * scrollMoveDistance;
        }
    }
}

void SceneViewer::AddMoveVector() {
    //Add scaled move vector
    objTransform.position += moveVector.x * camera.GetCameraRight() * moveSpeed * GetLevel()->currentDeltaTime
    + moveVector.y * camera.GetCameraForward() * moveSpeed * GetLevel()->currentDeltaTime
    + moveVector.z * camera.GetCameraUp() * moveSpeed * GetLevel()->currentDeltaTime;
}

void SceneViewer::Begin() {
    Empty::Begin();

    //Register inputs to listen for
    RegisterCursorCallback(this);
    RegisterButtonCallback(this, GLFW_MOUSE_BUTTON_RIGHT);
    RegisterButtonCallback(this, GLFW_MOUSE_BUTTON_MIDDLE);
    RegisterScrollCallback(this);
    RegisterKeyCallback(this, GLFW_KEY_W);
    RegisterKeyCallback(this, GLFW_KEY_A);
    RegisterKeyCallback(this, GLFW_KEY_S);
    RegisterKeyCallback(this, GLFW_KEY_D);
    RegisterKeyCallback(this, GLFW_KEY_E);
    RegisterKeyCallback(this, GLFW_KEY_R);
}

void SceneViewer::Tick() {
    Empty::Tick();
    AddMoveVector();
}

SceneViewer::~SceneViewer() {
    FileManager::WriteStringToLog("Destroying SceneViewer");
}
