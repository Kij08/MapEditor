//
// Created by Kij on 25/06/18.
//

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "../Renderer/RTypes.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 colour;
    glm::vec3 normal;
    glm::vec2 uv;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && colour == other.colour && uv == other.uv;
    }
};

//Hash function for custom struct usage with std::map
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.colour) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}


struct MeshBuffers {
    VmaBuffer vertexBuffer;
    VmaBuffer indexBuffer;
    VkDeviceAddress vertexBufferAddress;
};

struct MeshPrimitives {
    uint32_t startIndex;
    uint32_t count;
};

class Mesh {
public:
    Mesh(const MeshBuffers& mBuffer) {
        buffers = mBuffer;
    }

    const VkDeviceAddress& GetVertexBufferAddress() { return buffers.vertexBufferAddress; }
private:

    MeshBuffers buffers;

    //Vector holding all mesh primitives, Mesh will have multiple if it needs more than one material for example
    std::vector<MeshPrimitives> primitives;


};




#endif //MESH_H
