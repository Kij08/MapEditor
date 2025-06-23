//
// Created by Kij on 25/06/19.
//

#ifndef RDESCRIPTORS_H
#define RDESCRIPTORS_H

#include "Renderer.h"
#include <span>
#include <deque>

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void AddBinding(uint32_t binding, VkDescriptorType type);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct DescriptorAllocator {

    //Type and ratio. Ratio determines how many descriptors in each descriptor set.
    struct PoolSizeRatio{
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    //Max sets is how many descriptor sets can be allocated from this pool.
    void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void ClearDescriptors(VkDevice device);
    void DestroyPool(VkDevice device);

    //Allocates set from pool
    VkDescriptorSet AllocateDescriptorSet(VkDevice device, VkDescriptorSetLayout layout);
};

struct DescriptorAllocatorGrowable {
public:
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void ClearDescriptors(VkDevice device);
    void DestroyPools(VkDevice device);

    VkDescriptorSet AllocateDescriptorSet(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);
private:
    VkDescriptorPool GetPool(VkDevice device);
    VkDescriptorPool CreatePool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

    std::vector<PoolSizeRatio> ratios;
    std::vector<VkDescriptorPool> fullPools;
    std::vector<VkDescriptorPool> readyPools;
    uint32_t setsPerPool;

};

struct DescriptorWriter {
    std::deque<VkDescriptorImageInfo> imageInfos;
    std::deque<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;

    void WriteImage(int binding,VkImageView image,VkSampler sampler , VkImageLayout layout, VkDescriptorType type);
    void WriteBuffer(int binding,VkBuffer buffer,size_t size, size_t offset,VkDescriptorType type);

    void Clear();
    void UpdateSet(VkDevice device, VkDescriptorSet set);
};


#endif //RDESCRIPTORS_H
