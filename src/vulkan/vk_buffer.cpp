#define VMA_IMPLEMENTATION
#include "vulkan/vk_buffer.hpp"
#include <iostream>
#include <cstring>

VmaAllocator create_allocator(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device) {
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.instance = instance;
    allocator_info.physicalDevice = physical_device;
    allocator_info.device = device;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;

    VmaAllocator allocator = nullptr;
    if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS) {
        std::cerr << "Echec de la creation de l'allocateur VMA\n";
        return nullptr;
    }
    return allocator;
}

void destroy_allocator(VmaAllocator allocator) {
    vmaDestroyAllocator(allocator);
}

Buffer create_buffer(VmaAllocator allocator, VkDeviceSize size,
                       VkBufferUsageFlags usage, VmaMemoryUsage memory_usage) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = memory_usage;
    alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    Buffer result{};
    if (vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &result.buffer, &result.allocation, nullptr) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du buffer\n";
        result.buffer = VK_NULL_HANDLE;
    }
    return result;
}

void destroy_buffer(VmaAllocator allocator, const Buffer& buffer) {
    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}

Buffer create_vertex_buffer(VmaAllocator allocator, const std::vector<Vertex2DColor>& vertices) {
    VkDeviceSize size = sizeof(Vertex2DColor) * vertices.size();

    Buffer buffer = create_buffer(allocator, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO);

    void* data = nullptr;
    vmaMapMemory(allocator, buffer.allocation, &data);
    std::memcpy(data, vertices.data(), static_cast<size_t>(size));
    vmaUnmapMemory(allocator, buffer.allocation);

    return buffer;
}
