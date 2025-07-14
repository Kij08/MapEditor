//
// Created by Kij on 25/06/18.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "../Utils/AssetLoader.h"
#include "SceneViewer.h"

struct SceneNode {
    SceneNode* parent;
    std::vector<SceneNode*> children;

    std::shared_ptr<Empty> thisObject;

    bool isRoot = false;

    std::string GetObjName() {
        return thisObject->GetDisplayName();
    }

    void AddChild(SceneNode* child) {
        children.push_back(child);
    }

    SceneNode() {
        parent = nullptr;
        isRoot = true;
    };

    SceneNode(std::shared_ptr<Empty> e, SceneNode* p) {
        thisObject = e;
        parent = p;
        isRoot = false;
    };

    ~SceneNode() {
        //Loop through all children nodes and delete their children down the tree
        for (auto child : children) {
            delete child;
        }

        children.clear();
    }
};

class Scene {
protected:
    //List of all objects
    std::vector<std::shared_ptr<Object>> objects;

    //Asset manager for all objects in this scene
    AssetManager sceneAssetManager;
    SceneViewer sceneViewer;
private:
    SceneNode SceneGraphRoot;

public:
    Scene();
    ~Scene();

    void InitScene();

    template<class TObject>
    std::shared_ptr<TObject> CreateObject() {
        static_assert(std::is_base_of<Empty, TObject>::value, "Not an object");

        //Setup code for all objects
        std::shared_ptr<TObject> obj = std::make_shared<TObject>(this);
        objects.push_back(obj);

        //Allocated memory gets stored and deleted when the scene is deleted
        SceneNode* node = new SceneNode(obj, &SceneGraphRoot);
        SceneGraphRoot.AddChild(node);

        sceneAssetManager.LoadObject(obj.get());
        obj->Begin();
        return obj;
    }

    template<class TObject>
    std::shared_ptr<TObject> CreateObject(std::string model, std::string texture) {
        static_assert(std::is_base_of<Empty, TObject>::value, "Not an Object");

        //Setup code for all objects
        std::shared_ptr<TObject> obj = std::make_shared<TObject>(this, model, texture);
        objects.push_back(obj);

        //Allocated memory gets stored and deleted when the scene is deleted
        SceneNode* node = new SceneNode(obj, &SceneGraphRoot);
        SceneGraphRoot.AddChild(node);

        sceneAssetManager.LoadObject(obj.get());
        obj->Begin();
        return obj;
    }

    const std::vector<std::shared_ptr<Object>>& GetObjectList() { return objects; }
    SceneNode* GetSceneRoot() { return &SceneGraphRoot; }

    void Tick(float deltaTime);
    float currentDeltaTime = 0;

    glm::vec3 GetCameraForwardDirection() { return sceneViewer.GetCamera()->GetCameraForward(); };
    glm::vec3 GetCameraPosition() { return sceneViewer.GetTransform().position; };

};

#endif //SCENE_H
