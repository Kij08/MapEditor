//
// Created by Kij on 25/06/18.
//

#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include <vector>

class Mesh {
public:
    struct MeshData {
        glm::vec3 pos;
        glm::vec3 colour;
        glm::vec2 uv;
        glm::vec3 normal;
    };

    std::vector<MeshData> vertices;
    std::vector<uint32_t> indices;
};


#endif //MESH_H
