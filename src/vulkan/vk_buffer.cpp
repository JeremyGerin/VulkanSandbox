#define VMA_IMPLEMENTATION
#include "vulkan/vk_buffer.hpp"
#include <iostream>

bool create_allocator(VulkanContext& ctx) {
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.instance = ctx.instance;
    allocator_info.physicalDevice = ctx.physical_device;
    allocator_info.device = ctx.logical_device;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;

    if (vmaCreateAllocator(&allocator_info, &ctx.allocator) != VK_SUCCESS) {
        std::cerr << "Echec de la creation de l'allocateur VMA\n";
        return false;
    }
    return true;
}

void destroy_allocator(VulkanContext& ctx) {
    if (ctx.allocator != nullptr) {
        vmaDestroyAllocator(ctx.allocator);
        ctx.allocator = nullptr;
    }
}

bool create_buffer(VulkanContext& ctx, VkDeviceSize size,
                    VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, Buffer& out_buffer) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = memory_usage;
    alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    if (vmaCreateBuffer(ctx.allocator, &buffer_info, &alloc_info, &out_buffer.buffer, &out_buffer.allocation, nullptr) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du buffer\n";
        out_buffer.buffer = VK_NULL_HANDLE;
        out_buffer.allocation = VK_NULL_HANDLE;
        return false;
    }

    out_buffer.size_bytes = size;

    return true;
}

void destroy_buffer(VulkanContext& ctx, Buffer& buffer) {
    if (buffer.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(ctx.allocator, buffer.buffer, buffer.allocation);
        buffer.buffer = VK_NULL_HANDLE;
        buffer.allocation = VK_NULL_HANDLE;
    }
}

bool create_index_buffer(VulkanContext& ctx, const std::vector<uint32_t>& indices,
                          VkIndexType index_type, Buffer& out_buffer) {
    const void* src_data = nullptr;
    VkDeviceSize size = 0;

    // Conversion en uint16_t seulement si demandé explicitement
    std::vector<uint16_t> indices_16;
    if (index_type == VK_INDEX_TYPE_UINT16) {
        indices_16.reserve(indices.size());
        for (uint32_t i : indices) {
            indices_16.push_back(static_cast<uint16_t>(i));
        }
        src_data = indices_16.data();
        size = sizeof(uint16_t) * indices_16.size();
    } else {
        src_data = indices.data();
        size = sizeof(uint32_t) * indices.size();
    }

    if (!create_buffer(ctx, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, out_buffer)) {
        return false;
    }

    void* data = nullptr;
    if (vmaMapMemory(ctx.allocator, out_buffer.allocation, &data) != VK_SUCCESS) {
        destroy_buffer(ctx, out_buffer);
        return false;
    }
    std::memcpy(data, src_data, static_cast<size_t>(size));
    vmaUnmapMemory(ctx.allocator, out_buffer.allocation);

    out_buffer.count = static_cast<uint32_t>(indices.size());
    out_buffer.index_type = index_type;

    return true;
}

bool create_uniform_buffer(VulkanContext& ctx, VkDeviceSize size, Buffer& out_buffer) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                      | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VmaAllocationInfo allocation_info{};

    if (vmaCreateBuffer(ctx.allocator, &buffer_info, &alloc_info, &out_buffer.buffer, &out_buffer.allocation, &allocation_info) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du uniform buffer\n";
        out_buffer.buffer = VK_NULL_HANDLE;
        out_buffer.allocation = VK_NULL_HANDLE;
        return false;
    }

    out_buffer.size_bytes = size;
    out_buffer.mapped_data = allocation_info.pMappedData;

    return true;
}