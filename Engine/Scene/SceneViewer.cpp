//
// Created by Kij on 25/06/18.
//

#include "SceneViewer.h"

#include <iostream>
#include <bits/ostream.tcc>
#include <GLFW/glfw3.h>


SceneViewer::SceneViewer() {
    ViewerState = EViewerState::VIEWER_EDIT;
}

void SceneViewer::SetState(EViewerState newState) {
    ViewerState = newState;
}

void SceneViewer::RespondToButton(int key, int button, int scancode, int action, int mods) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            SetState(EViewerState::VIEWER_MOVE);
            break;
    }
    //Move viewer if in movement mode
    if (ViewerState == EViewerState::VIEWER_MOVE) {

    }
}

void SceneViewer::RespondToCursor(double xPos, double yPos) {
    //Move camera if in movement mode
    if (ViewerState == EViewerState::VIEWER_MOVE) {

    }
    std::cout << xPos << " " << yPos << std::endl;
}

void SceneViewer::Begin() {
    Empty::Begin();
    std::cout << "Begin play for scene viewer" << std::endl;
    RegisterCursorCallback(this);
    RegisterButtonCallback(this, GLFW_MOUSE_BUTTON_RIGHT);
}
