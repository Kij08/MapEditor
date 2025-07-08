//
// Created by Kij on 25/06/18.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "../Utils/AssetLoader.h"
#include "SceneViewer.h"

struct ObjectMaterialGrouping {

};

class Scene {

protected:
    //List of all objects, grouped by material
    std::vector<ObjectMaterialGrouping> materialGroupings;
    std::vector<std::shared_ptr<Object>> objects;

    //Asset manager for all objects in this scene
    AssetManager sceneAssetManager;
    SceneViewer sceneViewer;
private:

public:

    void InitScene();

    template<class TObject>
    std::shared_ptr<TObject> CreateObject() {
        static_assert(std::is_base_of<Object, TObject>::value, "Not an Object");

        //Setup code for all objects
        std::shared_ptr<TObject> obj = std::make_shared<Object>();
        obj->SetLevelReference(this);
        objects.push_back(obj);
        sceneAssetManager.LoadObject(obj.get());
        obj->Begin();
        return obj;
    }

    template<class TObject>
    std::shared_ptr<TObject> CreateObject(std::string model, std::string texture) {
        static_assert(std::is_base_of<Object, TObject>::value, "Not an Object");

        //Setup code for all objects
        std::shared_ptr<TObject> obj = std::make_shared<Object>(model, texture);
        obj->SetLevelReference(this);
        objects.push_back(obj);
        sceneAssetManager.LoadObject(obj.get());
        obj->Begin();
        return obj;
    }

    const std::vector<std::shared_ptr<Object>>& GetObjectList() { return objects; }

    void Tick();
};

#endif //SCENE_H
