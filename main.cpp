#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utils/FileManager.h"
#include "Engine/Utils/EngineUI.h"

void mainLoop() {

    Renderer& r = Renderer::Get();
    r.Startup();

    //Load all UI textures here
    EngineUI::LoadUIAssets();

    //Init empty scene
    Scene* s = new Scene();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    //Main render loop
    while (!glfwWindowShouldClose(r.GetWindow())) {
        //Delta time calulation
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents(); //Poll input callbacks
        s->Tick(deltaTime); //Tick level and all objects in it

        ImDrawData* guiDrawData = EngineUI::RenderImGuiElements(s); //Get data for UI elements
        r.DrawFrame(s->GetObjectList(), guiDrawData, s->GetCameraForwardDirection(), s->GetCameraPosition()); //Draw objects and UI
        //TODO: Pass just pointer to main camera maybe?
    }

    vkDeviceWaitIdle(r.GetDevice());
    delete s;
}

int main() {
    FileManager::OpenLogFile();
    FileManager::WriteStringToLog("Begin Editor");
    mainLoop();
    FileManager::WriteStringToLog("End Editor");
    FileManager::CloseLogFile();
}