//
// Created by Kij on 25/06/18.
//

#include "Scene.h"
#include "../Primitives/Object.h"

void Scene::InitScene() {
    sceneViewer.Begin();
    CreateObject<Object>();
}

void Scene::Tick(float deltaTime) {
    currentDeltaTime = deltaTime;
    sceneViewer.Tick();
    for (auto obj : objects) {
        obj->Tick();
    }
}


