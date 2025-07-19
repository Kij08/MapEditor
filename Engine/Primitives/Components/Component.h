//
// Created by Kij on 25/07/17.
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include "../ObjectTypes.h"

class Component {
public:
    explicit Component(class Empty* o);
    explicit Component(class Object* o);

    virtual ~Component() = default;
protected:
    Empty* Owner;

    Transform ComponentTransform;

public:

    Transform& GetComponentTransform() { return ComponentTransform; }
    void SetRelativeTransform(Transform t) { ComponentTransform = t; }

    //Call to render components UI implementation
    virtual void RenderComponentImGui(class Scene* s) = 0;
};



#endif //COMPONENT_H
