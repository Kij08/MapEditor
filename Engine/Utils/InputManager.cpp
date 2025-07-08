//
// Created by Kij on 25/07/07.
//

#include "InputManager.h"
#include "GLFW/glfw3.h"

void InputManager::InitInput(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, MousePositionCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
}

void InputManager::MousePositionCallback(GLFWwindow *window, double xPos, double yPos) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == Callback_MouseMove) {
            ic.callbackOwner->RespondToCursor(xPos, yPos);
        }
    }
}

void InputManager::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == button) {
            //-1 for values not used by mouse callbacks
            ic.callbackOwner->RespondToButton(-1, button, -1, action, mods);
        }
    }
}

void InputManager::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == Callback_MouseMove) {
            //-1 for values not used by key callbacks
            ic.callbackOwner->RespondToButton(key,-1, scancode, action, mods);
        }
    }
}
