//
// Created by Kij on 25/06/18.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "../Utils/AssetLoader.h"
#include "SceneViewer.h"
#include "../Primitives/Object.h"

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

    //Recursively adds nodes to imgui tree node and returns an int >= 0 if a tree node was clicked
    SceneNode* AddUITreeNode() {
        SceneNode* selectedNode = nullptr;
        if (isRoot == false) {
            ImGuiTreeNodeFlags flag = 0;
            if (children.empty()) {
                flag = ImGuiTreeNodeFlags_Leaf;
            }
            bool didStyling = false;
            if (thisObject->isSelectedInScene) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 50, 50, 255));
                didStyling = true;
            }
            if (ImGui::TreeNodeEx(GetObjName().c_str(), flag)) {

                //Store the selected object
                if (ImGui::IsItemClicked()) {
                    selectedNode = this;
                    thisObject->isSelectedInScene = true;
                }
                ImGui::TreePop();
            };
            if (didStyling) {
                ImGui::PopStyleColor();
            }
            SceneNode* node;
            for (auto child : children) {
                node = child->AddUITreeNode();
                if (node) {
                    selectedNode = node;
                }
            }
        }
        else {
            //If this is the scene root then create the root tree node
            ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;

            if (ImGui::TreeNodeEx("Scene Root", flag))
            {
                ImGui::TreePop();
            }
            SceneNode* node;
            for (auto child : children) {
                node = child->AddUITreeNode();
                if (node) {
                    selectedNode = node;
                }
            }
        }
        return selectedNode;
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
        //Recurse through all children nodes and delete their children down the tree
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

    void AddDefinedObjectToScene(const ObjectDefinition& objectDef);

public:
    Scene();
    explicit Scene(const std::vector<ObjectDefinition>& objectDefinitions);
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

        obj->Begin();
        return obj;
    }

    template<class TObject>
    std::shared_ptr<TObject> CreateObject(std::string name) {
        static_assert(std::is_base_of<Empty, TObject>::value, "Not an object");

        //Setup code for all objects
        std::shared_ptr<TObject> obj = std::make_shared<TObject>(this, name);
        objects.push_back(obj);

        //Allocated memory gets stored and deleted when the scene is deleted
        SceneNode* node = new SceneNode(obj, &SceneGraphRoot);
        SceneGraphRoot.AddChild(node);

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

        obj->Begin();
        return obj;
    }

    template<class TObject>
    std::shared_ptr<TObject> CreateObject(std::string name, Transform t, const MeshComponentDefinition& mc) {
        static_assert(std::is_base_of<Empty, TObject>::value, "Not an Object");

        //Setup code for all objects
        std::shared_ptr<TObject> obj = std::make_shared<TObject>(this, t, mc);
        objects.push_back(obj);

        //Allocated memory gets stored and deleted when the scene is deleted
        SceneNode* node = new SceneNode(obj, &SceneGraphRoot);
        SceneGraphRoot.AddChild(node);

        obj->Begin();
        return obj;
    }

    const std::vector<std::shared_ptr<Object>>& GetObjectList() { return objects; }
    SceneNode* GetSceneRoot() { return &SceneGraphRoot; }
    AssetManager* GetSceneAssetManager() { return &sceneAssetManager; }

    void Tick(float deltaTime);
    float currentDeltaTime = 0;

    SceneViewer* GetViewer() { return &sceneViewer; }
    glm::vec3 GetCameraForwardDirection() { return sceneViewer.GetCamera()->GetCameraForward(); };
    glm::vec3 GetCameraPosition() { return sceneViewer.GetTransform().position; };

};

#endif //SCENE_H
