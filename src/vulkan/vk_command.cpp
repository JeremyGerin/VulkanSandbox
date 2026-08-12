#include "vulkan/vk_command.hpp"
#include <iostream>

VkCommandPool create_command_pool(VkDevice device, uint32_t graphics_family_index) {
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = graphics_family_index;

    VkCommandPool pool = VK_NULL_HANDLE;
    if (vkCreateCommandPool(device, &pool_info, nullptr, &pool) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du command pool\n";
        return VK_NULL_HANDLE;
    }

    return pool;
}

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count) {
    std::vector<VkCommandBuffer> buffers(count);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;

    if (vkAllocateCommandBuffers(device, &alloc_info, buffers.data()) != VK_SUCCESS) {
        std::cerr << "Echec de l'allocation des command buffers\n";
        return {};
    }

    return buffers;
}

void record_command_buffer(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkExtent2D extent, VkPipeline pipeline, VkBuffer vertex_buffer, uint32_t vertex_count) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        std::cerr << "Echec du begin command buffer\n";
        return;
    }

    VkClearValue clear_color = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass;
    render_pass_info.framebuffer = framebuffer;
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = extent;
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkBuffer vertex_buffers[] = { vertex_buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

    vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        std::cerr << "Echec du end command buffer\n";
    }
}
