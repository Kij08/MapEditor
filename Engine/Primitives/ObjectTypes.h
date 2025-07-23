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

    //Transforms a comma seperated transform string into a Transform type
    static Transform StringToTransform(std::string s) {
        int transformValues[9];
        for (int i = 0; i < 9; i++) {
            size_t bitEndPos = s.find_first_of(',');
            std::string transformBit = s.substr(0, bitEndPos);
            s = s.substr(bitEndPos + 1);

            transformValues[i] = std::stoi(transformBit);
        }
        return {.position = {transformValues[0], transformValues[1], transformValues[2]},
        .rotation = {transformValues[3], transformValues[4], transformValues[5]},
        .scale = {transformValues[6], transformValues[7], transformValues[8]}};
    }
};

//Definitions for reading objects and components from files
struct BaseComponentDefinition {
    std::string componentName;
    std::string className;
    Transform componentTransform = {};
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
    std::vector<BaseComponentDefinition*> ComponentDefinitions;
};


#endif //OBJECTTYPES_H
