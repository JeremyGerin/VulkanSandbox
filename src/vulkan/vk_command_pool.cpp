#include "vulkan/vk_command_pool.hpp"
#include <iostream>

bool create_command_pool(VulkanContext& ctx) {
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = ctx.queue_family_indices.graphics.value();

    if (vkCreateCommandPool(ctx.logical_device, &pool_info, nullptr, &ctx.command_pool) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du command pool\n";
        return false;
    }

    return true;
}

void destroy_command_pool(VulkanContext& ctx) {
    if (ctx.command_pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(ctx.logical_device, ctx.command_pool, nullptr);
        ctx.command_pool = VK_NULL_HANDLE;
    }
}
