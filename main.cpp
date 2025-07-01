#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
void mainLoop(Scene* scene) {

    Renderer& r = Renderer::Get();
    while (!glfwWindowShouldClose(r.GetWindow())) {

        glfwPollEvents();

        r.DrawFrame(scene->GetObjectList());
    }

    vkDeviceWaitIdle(r.GetDevice());
}

int main() {
    Scene s;
    s.InitScene();
    mainLoop(&s);
}