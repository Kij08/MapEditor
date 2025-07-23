//
// Created by Kij on 25/07/08.
//

#ifndef EMPTY_H
#define EMPTY_H

#include <memory>
#include <string>
#include <vector>

#include "ObjectTypes.h"
#include "Components/Component.h"

class Scene;
//Base class for anything in the scene
class Empty {
public:
    Empty() = delete;
    explicit Empty(Scene* s);

    Empty(Scene* s, Transform t);
    Empty(Scene* s, const std::string& name);
    Empty(Scene* s, const std::string& name, Transform t);
protected:
    Transform objTransform;
    Scene* SceneRef;

    std::string ObjectDisplayName;
    uint32_t objectID;

    //List of registered components on this scene object
    std::vector<Component*> Components;
public:
    const Transform GetTransform() { return objTransform; };
    void SetTransform(Transform t) { objTransform = t; };

    Scene* GetLevel() { return SceneRef; };

    std::string GetDisplayName() { return ObjectDisplayName; };

    void AddComponent(Component* c) { Components.push_back(c); };
    const std::vector<Component*>& GetComponents() { return Components; };

    virtual void Tick() {};
    virtual void Begin() {};

    virtual ObjectDefinition GetObjectDefinition() { return ObjectDefinition{}; }

    virtual ~Empty() = default;

    bool isSelectedInScene = false;
};



#endif //EMPTY_H
