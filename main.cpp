#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"

void mainLoop() {

    Renderer& r = Renderer::Get();
    r.Startup();

    Scene s;
    s.InitScene();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


    while (!glfwWindowShouldClose(r.GetWindow())) {
        //Delta time calulation
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents(); //Poll input callbacks
        s.Tick(deltaTime); //Tick level and all objects in it

        ImDrawData* guiDrawData = r.RenderImGUIElements(&s); //Get data for UI elements
        r.DrawFrame(s.GetObjectList(), guiDrawData, s.GetCameraForwardDirection(), s.GetCameraPosition()); //Draw objects and UI
        //TODO: Pass just pointer to camera maybe?
    }

    vkDeviceWaitIdle(r.GetDevice());
}

int main() {

    mainLoop();
}