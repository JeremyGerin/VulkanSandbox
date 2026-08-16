#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_buffer.hpp"

bool create_descriptor_pool(VulkanContext& ctx, const std::vector<VkDescriptorPoolSize>& pool_sizes, uint32_t max_sets, VkDescriptorPool& out_pool);

void destroy_descriptor_pool(VulkanContext& ctx, VkDescriptorPool& pool);

bool allocate_descriptor_sets(VulkanContext& ctx, VkDescriptorPool pool, VkDescriptorSetLayout layout, uint32_t count, std::vector<VkDescriptorSet>& out_sets);

void update_uniform_descriptor_set(VulkanContext& ctx, VkDescriptorSet set, uint32_t binding, const Buffer& buffer, VkDeviceSize range);
