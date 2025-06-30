//
// Created by Kij on 25/06/18.
//

#include "Scene.h"

void Scene::InitScene() {
}

template<class TObject>
TObject* Scene::CreateObject() {
    static_assert(std::is_base_of<Object, TObject>::value, "Not an Object");

    TObject* obj = new TObject();
    obj->SetLevelReference(this);
    objects.insert(obj);
    sceneAssetManager.LoadObject(obj);
    return obj;
}