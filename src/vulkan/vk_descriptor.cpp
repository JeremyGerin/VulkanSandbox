#include "vulkan/vk_descriptor.hpp"

#include <iostream>

bool create_descriptor_pool(VulkanContext& ctx, const std::vector<VkDescriptorPoolSize>& pool_sizes, uint32_t max_sets, VkDescriptorPool& out_pool) {
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = max_sets;

    if (vkCreateDescriptorPool(ctx.logical_device, &pool_info, nullptr, &out_pool) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du descriptor pool\n";
        return false;
    }

    return true;
}

void destroy_descriptor_pool(VulkanContext& ctx, VkDescriptorPool& pool) {
    if (pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(ctx.logical_device, pool, nullptr);
        pool = VK_NULL_HANDLE;
    }
}

bool allocate_descriptor_sets(VulkanContext& ctx, VkDescriptorPool pool, VkDescriptorSetLayout layout, uint32_t count, std::vector<VkDescriptorSet>& out_sets) {
    std::vector<VkDescriptorSetLayout> layouts(count, layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = count;
    alloc_info.pSetLayouts = layouts.data();

    out_sets.resize(count);

    if (vkAllocateDescriptorSets(ctx.logical_device, &alloc_info, out_sets.data()) != VK_SUCCESS) {
        std::cerr << "Echec de l'allocation des descriptor sets\n";
        return false;
    }

    return true;
}

void update_uniform_descriptor_set(VulkanContext& ctx, VkDescriptorSet set, uint32_t binding, const Buffer& buffer, VkDeviceSize range) {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = buffer.buffer;
    buffer_info.offset = 0;
    buffer_info.range = range;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(ctx.logical_device, 1, &write, 0, nullptr);
}
