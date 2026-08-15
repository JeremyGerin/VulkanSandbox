#include "vulkan/vk_framebuffer.hpp"
#include <iostream>

bool create_framebuffers(VulkanContext& ctx, SwapchainContext& swpch_ctx) {
    swpch_ctx.framebuffers.resize(swpch_ctx.image_views.size());

    for (size_t i = 0; i < swpch_ctx.image_views.size(); ++i) {
        VkImageView attachments[] = { swpch_ctx.image_views[i] };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = swpch_ctx.render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = swpch_ctx.extent.width;
        framebuffer_info.height = swpch_ctx.extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(ctx.logical_device, &framebuffer_info, nullptr, &swpch_ctx.framebuffers[i]) != VK_SUCCESS) {
            std::cerr << "Echec de la creation du framebuffer " << i << "\n";
            return false;
        }
    }

    return true;
}

void destroy_framebuffers(VulkanContext& ctx, SwapchainContext& swpch_ctx) {
    for (VkFramebuffer fb : swpch_ctx.framebuffers) {
        if (fb != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(ctx.logical_device, fb, nullptr);
        }
    }
    swpch_ctx.framebuffers.clear();
}