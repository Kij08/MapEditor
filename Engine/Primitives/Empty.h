//
// Created by Kij on 25/07/08.
//

#ifndef EMPTY_H
#define EMPTY_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <string>


struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class Scene;
//Base class for anything in the scene
class Empty {
public:
    Empty() = delete;
    explicit Empty(Scene* s);

    Empty(Scene* s, Transform t);
protected:
    Transform objTransform;
    Scene* SceneRef;

    std::string ObjectDisplayName;
    uint32_t objectID;

public:
    const Transform GetTransform() { return objTransform; };
    void SetTransform(Transform t) { objTransform = t; };

    Scene* GetLevel() { return SceneRef; };

    std::string GetDisplayName() { return ObjectDisplayName; };

    virtual void Tick() {};
    virtual void Begin() {};

    virtual ~Empty() = default;
};



#endif //EMPTY_H
