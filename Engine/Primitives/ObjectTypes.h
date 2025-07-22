//
// Created by Kij on 25/07/17.
//

#ifndef OBJECTTYPES_H
#define OBJECTTYPES_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <string>

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

//Definitions for reading objects and components from files
struct BaseComponentDefinition {
    std::string componentName;
    std::string className;
    Transform transform = {};
    virtual ~BaseComponentDefinition() = default;
};

struct MeshComponentDefinition : BaseComponentDefinition {
    std::string meshPath;
    std::string texturePath;
    std::string materialName;
};

struct ObjectDefinition {
    std::string objectName;
    std::string className;
    Transform transform = {};

    //list of component definitions
    std::vector<BaseComponentDefinition> ComponentDefinitions;
};


#endif //OBJECTTYPES_H
