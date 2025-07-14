//
// Created by Kij on 25/07/08.
//

#include "Empty.h"
#include "../Scene/Scene.h"

Empty::Empty(Scene* s) : SceneRef(s),
        objTransform({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)}),
        objectID(s->GetObjectList().size()), ObjectDisplayName("Object" + std::to_string(s->GetObjectList().size())) {

}

Empty::Empty(Scene* s, Transform t) : SceneRef(s),
    objTransform(t), objectID(s->GetObjectList().size()), ObjectDisplayName("Object" + std::to_string(s->GetObjectList().size())) {

}