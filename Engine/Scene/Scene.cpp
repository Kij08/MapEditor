//
// Created by Kij on 25/06/18.
//

#include "Scene.h"
#include "../Primitives/Object.h"

void Scene::InitScene() {
    CreateObject<Object>();
}

template<class TObject>
std::shared_ptr<TObject> Scene::CreateObject() {
    static_assert(std::is_base_of<Object, TObject>::value, "Not an Object");

    //Setup code for all objects
    std::shared_ptr<TObject> obj = std::make_shared<Object>();
    obj->SetLevelReference(this);
    objects.push_back(obj);
    sceneAssetManager.LoadObject(obj.get());
    return obj;
}