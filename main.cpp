#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"

#include <iostream>
void mainLoop() {

    Renderer& r = Renderer::Get();
    r.Startup();

    Scene s;
    s.InitScene();

    while (!glfwWindowShouldClose(r.GetWindow())) {

        glfwPollEvents();

        r.DrawFrame(s.GetObjectList());
    }

    vkDeviceWaitIdle(r.GetDevice());
}

int main() {

    mainLoop();
}