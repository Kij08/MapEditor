//
// Created by Kij on 25/06/18.
//

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "../Renderer/RTypes.h"

//Struct interleaved for allignment purposes. vec3 + float is 16, then again another 16, then vec4 is another 16. This results in 48 bytes and no padding
struct alignas(16) Vertex {
    glm::vec3 pos;
    float xUV;
    glm::vec3 normal;
    float yUV;
    glm::vec4 colour;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && colour == other.colour && xUV == other.xUV && yUV == other.yUV;
    }
};

//Hash function for custom struct usage with std::map
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.colour) << 1)) >> 1) ^
                (hash<glm::vec2>()(glm::vec2(vertex.xUV, vertex.yUV)) << 1);
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
    const VmaBuffer& GetIndexBuffer() { return buffers.indexBuffer; }

    std::vector<MeshPrimitives> primitives;

private:

    MeshBuffers buffers;

    //Vector holding all mesh primitives, Mesh will have multiple if it needs more than one material for example


};




#endif //MESH_H
