//
// Created by Kij on 25/06/18.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "../Renderer/RTypes.h"

struct TextureAllocation {
    VkImage TextureImage;
    VmaAllocation TextureMem;
    VkImageView TextureImageView;
    VkExtent3D ImageExtent;
    VkFormat ImageFormat;
    VkSampler TextureSampler;
};

class Texture {
public:

    Texture(const TextureAllocation& tAlloc) : Allocation(tAlloc), TextureName("DefaultTexture") {}

    Texture(const TextureAllocation& tAlloc, std::string name) : Allocation(tAlloc), TextureName(name) {}

    TextureAllocation& GetAllocation() { return Allocation; }
    std::string GetName() { return TextureName; }
    void SetName(std::string name) { TextureName = name; }
private:
    TextureAllocation Allocation;
    std::string TextureName;

};



#endif //TEXTURE_H
