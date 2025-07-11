//
// Created by Kij on 25/07/07.
//

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <vector>

//Readability for mouse moving callbacks
constexpr int Callback_MouseMove = -1;
constexpr int Callback_Scroll = -2;

class GLFWwindow;

struct InputCallback {
    class IInputResponse* callbackOwner;
    int inputToRegister;
};

namespace InputManager {

    void InitInput(GLFWwindow* window);

    void MousePositionCallback(GLFWwindow* window, double xPos, double yPos);
    void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    //Store all bound callbacks
    inline std::vector<InputCallback> inputCallbacks;
};

class IInputResponse {
public:
    virtual ~IInputResponse() {
        for (int i : CallbackIndices) {
            //InputManager::inputCallbacks[i];
        }
    }; //TODO: Delete callbacks and resize array
    virtual void RespondToMouseButton(GLFWwindow* window, int button, int action, int mods) = 0;
    virtual void RespondToKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
    virtual void RespondToCursor(GLFWwindow* window, double xPos, double yPos) = 0;
    virtual void RespondToScroll(GLFWwindow* window, double xOffset, double yOffset) = 0;

protected:
    void RegisterCursorCallback(IInputResponse* callbackOwner) {
        //Bind callback and store index to that callback locally so we can clean up later
        CallbackIndices.push_back(static_cast<int>(InputManager::inputCallbacks.size()));
        InputManager::inputCallbacks.push_back(InputCallback{ .callbackOwner=callbackOwner, .inputToRegister = Callback_MouseMove });
    };
    void RegisterScrollCallback(IInputResponse* callbackOwner) {
        //Bind callback and store index to that callback locally so we can clean up later
        CallbackIndices.push_back(static_cast<int>(InputManager::inputCallbacks.size()));
        InputManager::inputCallbacks.push_back(InputCallback{ .callbackOwner=callbackOwner, .inputToRegister = Callback_Scroll });
    };
    void RegisterButtonCallback(IInputResponse* callbackOwner, int buttonToRegister) {
        CallbackIndices.push_back(static_cast<int>(InputManager::inputCallbacks.size()));
        InputManager::inputCallbacks.push_back(InputCallback{ .callbackOwner=callbackOwner, .inputToRegister = buttonToRegister });
    };
    void RegisterKeyCallback(IInputResponse* callbackOwner, int keyToRegister) {
        CallbackIndices.push_back(static_cast<int>(InputManager::inputCallbacks.size()));
        InputManager::inputCallbacks.push_back(InputCallback{ .callbackOwner=callbackOwner, .inputToRegister = keyToRegister });
    };

private:
    //List of all callbacks that this object has bound
    std::vector<int> CallbackIndices;
};



#endif //INPUTMANAGER_H
