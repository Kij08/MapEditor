//
// Created by Kij on 25/06/18.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "../Renderer/Renderer.h"

struct TextureAllocation {
    VkImage TextureImage;
    VmaAllocation TextureMem;
    VkImageView TextureImageView;
    VkExtent3D ImageExtent;
    VkFormat ImageFormat;
    VkSampler TextureSampler;
};

struct Texture {


    TextureAllocation* Allocation;


};



#endif //TEXTURE_H
