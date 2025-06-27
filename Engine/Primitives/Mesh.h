//
// Created by Kij on 25/06/18.
//

#ifndef MESH_H
#define MESH_H

#include <array>
#include <vector>
#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"

class Mesh {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 colour;
        glm::vec2 uv;
        glm::vec3 normal;
    };

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

};




#endif //MESH_H
