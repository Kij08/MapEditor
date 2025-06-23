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
    //Keep these on texture

    //Put these on asset loaded
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void CreateTextureImageView(int objTexIndex);

    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void CreateTextureSampler();

};



#endif //TEXTURE_H
