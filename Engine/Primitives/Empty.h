//
// Created by Kij on 25/07/08.
//

#ifndef EMPTY_H
#define EMPTY_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>


class Scene;

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

//Base class for anything in the scene
class Empty {
public:
    Empty() = delete;
    explicit Empty(Scene* s) : SceneRef(s),
            objTransform({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)}){}
    Empty(Scene* s, Transform t) : SceneRef(s),
        objTransform(t){}
protected:
    Transform objTransform;
    Scene* SceneRef;

public:
    const Transform GetTransform() { return objTransform; };
    void SetTransform(Transform t) { objTransform = t; };

    Scene* GetLevel() { return SceneRef; };

    virtual void Tick() {};
    virtual void Begin() {};

    virtual ~Empty() = default;
};



#endif //EMPTY_H
