#include "vulkan/vk_framebuffer.hpp"
#include <iostream>

std::vector<VkFramebuffer> create_framebuffers(VkDevice device, VkRenderPass render_pass, const Swapchain& swapchain) {
    std::vector<VkFramebuffer> framebuffers(swapchain.image_views.size());

    for (size_t i = 0; i < swapchain.image_views.size(); ++i) {
        VkImageView attachments[] = { swapchain.image_views[i] };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = swapchain.extent.width;
        framebuffer_info.height = swapchain.extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            std::cerr << "Echec de la creation du framebuffer " << i << "\n";
            framebuffers[i] = VK_NULL_HANDLE;
        }
    }

    return framebuffers;
}

void destroy_framebuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers) {
    for (VkFramebuffer fb : framebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }
}
