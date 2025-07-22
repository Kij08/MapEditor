//
// Created by Kij on 25/06/18.
//

#include "Scene.h"
#include "../Primitives/Object.h"

Scene::Scene() : sceneViewer(this) {

}

void Scene::InitScene() {
    sceneViewer.Begin();
}

void Scene::Tick(float deltaTime) {
    currentDeltaTime = deltaTime;
    sceneViewer.Tick();
    for (auto obj : objects) {
        obj->Tick();
    }
}

Scene::~Scene() {
}

