//
// Created by Kij on 25/07/08.
//

#ifndef EMPTY_H
#define EMPTY_H

#include <glm/vec3.hpp>


class Scene;

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

//Base class for anything in the scene
class Empty {
protected:
    Transform objTransform;
    Scene* SceneRef;

public:
    const Transform GetTransform() { return objTransform; };
    void SetTransform(Transform t) { objTransform = t; };

    void SetLevelReference(Scene* scene) { SceneRef = scene; }
    Scene* GetLevel() { return SceneRef; };

    virtual void Tick() {};
    virtual void Begin() {};
};



#endif //EMPTY_H
