#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vector>
#include <cstring>
#include "vulkan/vk_context.hpp"

bool create_allocator(VulkanContext& ctx);
void destroy_allocator(VulkanContext& ctx);

struct Buffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    uint32_t count = 0;
    VkDeviceSize size_bytes = 0;
    VkIndexType index_type = VK_INDEX_TYPE_UINT32;
};

bool create_buffer(VulkanContext& ctx, VkDeviceSize size,
                    VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, Buffer& out_buffer);
void destroy_buffer(VulkanContext& ctx, Buffer& buffer);

template<typename T>
bool create_vertex_buffer(VulkanContext& ctx, const std::vector<T>& vertices, Buffer& out_buffer) {
    VkDeviceSize size = sizeof(T) * vertices.size();

    if (!create_buffer(ctx, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, out_buffer)) {
        return false;
    }

    void* data = nullptr;
    if (vmaMapMemory(ctx.allocator, out_buffer.allocation, &data) != VK_SUCCESS) {
        destroy_buffer(ctx, out_buffer);
        return false;
    }
    std::memcpy(data, vertices.data(), static_cast<size_t>(size));
    vmaUnmapMemory(ctx.allocator, out_buffer.allocation);

    out_buffer.count = static_cast<uint32_t>(vertices.size());

    return true;
}

bool create_index_buffer(VulkanContext& ctx, const std::vector<uint32_t>& indices, VkIndexType index_type, Buffer& out_buffer);