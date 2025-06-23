#include "Engine/Renderer/Renderer.h"

void mainLoop(Renderer* r) {
    while (!glfwWindowShouldClose(r->GetWindow())) {

        glfwPollEvents();

        //lvl->SimulateWorld();
        r->DrawFrame(lvl->GetWorldList());
    }

    vkDeviceWaitIdle(r->GetDevice());
}

int main() {
    Renderer r;

    r.Startup();

    mainLoop(&r);

    r.Terminate();
}