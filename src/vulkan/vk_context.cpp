#include "vulkan/vk_context.hpp"

#include "vulkan/vk_core.hpp"

bool create_vulkan_context(VulkanContext& ctx, int width, int height, const char* title) {
    return create_window(ctx, width, height, title) &&
        create_instance(ctx, title) &&
        setup_debug_messenger(ctx) &&
        create_surface(ctx) &&
        pick_physical_device(ctx) && 
        create_logical_device(ctx) &&
        create_command_pool(ctx) &&
        create_allocator(ctx);
}

void destroy_vulkan_context(VulkanContext& ctx) {
    destroy_allocator(ctx);
    destroy_command_pool(ctx);
    destroy_logical_device(ctx);
    destroy_surface(ctx);
    destroy_debug_messenger(ctx);
    destroy_instance(ctx);
    destroy_window(ctx);
}
