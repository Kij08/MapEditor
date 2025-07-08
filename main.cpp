#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"

void mainLoop() {

    Renderer& r = Renderer::Get();
    r.Startup();

    Scene s;
    s.InitScene();

    while (!glfwWindowShouldClose(r.GetWindow())) {

        glfwPollEvents(); //Poll input callbacks
        s.Tick(); //Tick level and all objects in it
        ImDrawData* guiDrawData = r.RenderImGUIElements(&s); //Get data for UI elements
        r.DrawFrame(s.GetObjectList(), guiDrawData); //Draw objects and UI

    }

    vkDeviceWaitIdle(r.GetDevice());
}

int main() {

    mainLoop();
}