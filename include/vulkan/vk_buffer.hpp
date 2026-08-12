#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vector>
#include "vk_pipeline.hpp"

struct Buffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

VmaAllocator create_allocator(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device);
void destroy_allocator(VmaAllocator allocator);

Buffer create_buffer(VmaAllocator allocator, VkDeviceSize size,
                       VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
void destroy_buffer(VmaAllocator allocator, const Buffer& buffer);

Buffer create_vertex_buffer(VmaAllocator allocator, const std::vector<Vertex2DColor>& vertices);