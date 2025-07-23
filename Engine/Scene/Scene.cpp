//
// Created by Kij on 25/06/18.
//

#include "Scene.h"
#include "../Primitives/Object.h"
#include "../Utils/FileManager.h"

Scene::Scene() : sceneViewer(this) {
    FileManager::WriteStringToLog("Load Scene");
    InitScene();
}

Scene::Scene(const std::vector<ObjectDefinition> &objectDefinitions) : sceneViewer(this) {
    FileManager::WriteStringToLog("Load Scene");
    InitScene();

    for (auto& def : objectDefinitions) {
        AddDefinedObjectToScene(def);

    }
}

void Scene::AddDefinedObjectToScene(const ObjectDefinition &objectDef) {
    std::vector<Component*> components;
    if (objectDef.className == FileManager::EmptyClass) {
        //s->CreateObject<Empty>(objectDef.objectName); TODO: Empty implementation
    }
    else if (objectDef.className == FileManager::ObjectClass) {
        MeshComponentDefinition meshDef;
        for (auto componentDef : objectDef.ComponentDefinitions) {
            if (auto meshComp = dynamic_cast<MeshComponentDefinition*>(componentDef)) {
                meshDef = *meshComp;
            }
        }

        CreateObject<Object>(objectDef.objectName, objectDef.transform, meshDef);

        //After using all the component definitions make sure to delete them here
        for (auto componentDef : objectDef.ComponentDefinitions) {
            delete componentDef;
        }
    }
}

void Scene::InitScene() {
    sceneViewer.Begin();
}

void Scene::Tick(float deltaTime) {
    currentDeltaTime = deltaTime;
    sceneViewer.Tick();
    for (const auto& obj : objects) {
        obj->Tick();
    }
}

Scene::~Scene() {
    FileManager::WriteStringToLog("Delete Scene");
}

