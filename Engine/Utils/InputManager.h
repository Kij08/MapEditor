//
// Created by Kij on 25/07/07.
//

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <vector>

//Readability for mouse moving callbacks
constexpr int Callback_MouseMove = -1;

class GLFWwindow;

struct InputCallback {
    class IInputResponse* callbackOwner;
    int inputToRegister;
};

namespace InputManager {

    void InitInput(GLFWwindow* window);

    void MousePositionCallback(GLFWwindow* window, double xPos, double yPos);
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    //Store all bound callbacks
    inline std::vector<InputCallback> inputCallbacks;
};

class IInputResponse {
public:
    virtual ~IInputResponse() = default; //TODO: Remove all bound callbacks from list
    virtual void RespondToButton(int key, int button, int scancode, int action, int mods) = 0;
    virtual void RespondToCursor(double xPos, double yPos) = 0;

protected:
    void RegisterCursorCallback(IInputResponse* callbackOwner) {
        //Bind callback and store index to that callback locally so we can clean up later
        callbackIndices.push_back(static_cast<int>(InputManager::inputCallbacks.size()));
        InputManager::inputCallbacks.push_back(InputCallback{ .callbackOwner=callbackOwner, .inputToRegister = Callback_MouseMove });
    };
    void RegisterButtonCallback(IInputResponse* callbackOwner, int buttonToRegister) {
        callbackIndices.push_back(static_cast<int>(InputManager::inputCallbacks.size()));
        InputManager::inputCallbacks.push_back(InputCallback{ .callbackOwner=callbackOwner, .inputToRegister = buttonToRegister });
    };

private:
    //List of all callbacks that this object has bound
    std::vector<int> callbackIndices;
};



#endif //INPUTMANAGER_H
