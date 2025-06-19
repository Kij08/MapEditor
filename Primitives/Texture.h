//
// Created by Kij on 25/06/18.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "vulkan/vulkan_core.h"

class Texture {
    private:

    VkImage TextureImage;
    VkDeviceMemory TextureImageMemory;
    VkImageView TextureImageView;
    VkSampler TextureSampler;

};



#endif //TEXTURE_H
