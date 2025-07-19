//
// Created by Kij on 25/07/17.
//

#include "Component.h"
#include "../Empty.h"
#include "../Object.h"

Component::Component(Empty *o) : Owner(o), ComponentTransform({glm::vec3(0), glm::vec3(0), glm::vec3(1)}) {
    o->AddComponent(this);
}

Component::Component(Object *o) : Owner(o), ComponentTransform({glm::vec3(0), glm::vec3(0), glm::vec3(1)}) {
    o->AddComponent(this);

}
