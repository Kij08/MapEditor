//
// Created by Kij on 25/07/07.
//

#include "InputManager.h"
#include "GLFW/glfw3.h"

void InputManager::InitInput(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, MousePositionCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

void InputManager::MousePositionCallback(GLFWwindow *window, double xPos, double yPos) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == Callback_MouseMove) {
            ic.callbackOwner->RespondToCursor(window, xPos, yPos);
        }
    }
}

void InputManager::ScrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == Callback_Scroll) {
            ic.callbackOwner->RespondToScroll(window, xOffset, yOffset);
        }
    }
}

void InputManager::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == button) {
            ic.callbackOwner->RespondToMouseButton(window, button, action, mods);
        }
    }
}

void InputManager::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    for (InputCallback ic : inputCallbacks) {
        if (ic.inputToRegister == Callback_MouseMove) {
            ic.callbackOwner->RespondToKey(window, key, scancode, action, mods);
        }
    }
}
