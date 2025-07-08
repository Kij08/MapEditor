//
// Created by Kij on 25/07/01.
//

#ifndef RTYPES_H
#define RTYPES_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "../../include/vk_mem_alloc.h"
#include "../../include/imgui/imgui.h"


struct VmaBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};


#endif //RTYPES_H
