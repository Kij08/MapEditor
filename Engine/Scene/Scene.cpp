//
// Created by Kij on 25/06/18.
//

#include "Scene.h"
#include "../Primitives/Object.h"

void Scene::InitScene() {
    //CreateObject<Object>();
}

template<class TObject>
TObject* Scene::CreateObject() {
    static_assert(std::is_base_of<Object, TObject>::value, "Not an Object");

    TObject* obj = new TObject();
    //Setup code for all objects
    obj->SetLevelReference(this);
    //objects.push_back(std::make_shared<Object>(obj));
    sceneAssetManager.LoadObject(obj);
    return obj;
}