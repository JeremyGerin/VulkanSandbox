#include "vulkan/vk_frame.hpp"
#include <iostream>

bool create_command_buffers(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t count) {
    frame_ctx.command_buffers.resize(count);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = ctx.command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;

    if (vkAllocateCommandBuffers(ctx.logical_device, &alloc_info, frame_ctx.command_buffers.data()) != VK_SUCCESS) {
        std::cerr << "Echec de l'allocation des command buffers\n";
        frame_ctx.command_buffers.clear();
        return false;
    }

    return true;
}

void destroy_command_buffers(VulkanContext& ctx, FrameContext& frame_ctx) {
    if (!frame_ctx.command_buffers.empty()) {
        vkFreeCommandBuffers(ctx.logical_device, ctx.command_pool,
            static_cast<uint32_t>(frame_ctx.command_buffers.size()),
            frame_ctx.command_buffers.data());
        frame_ctx.command_buffers.clear();
    }
}

bool create_sync_objects(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t swapchain_image_count) {
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    bool ok = true;

    frame_ctx.image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    frame_ctx.in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        ok = ok && vkCreateSemaphore(ctx.logical_device, &semaphore_info, nullptr, &frame_ctx.image_available_semaphores[i]) == VK_SUCCESS;
        ok = ok && vkCreateFence(ctx.logical_device, &fence_info, nullptr, &frame_ctx.in_flight_fences[i]) == VK_SUCCESS;
    }

    frame_ctx.render_finished_semaphores.resize(swapchain_image_count);
    for (uint32_t i = 0; i < swapchain_image_count; ++i) {
        ok = ok && vkCreateSemaphore(ctx.logical_device, &semaphore_info, nullptr, &frame_ctx.render_finished_semaphores[i]) == VK_SUCCESS;
    }

    if (!ok) {
        std::cerr << "Echec de la creation des objets de synchronisation\n";
    }

    return ok;
}

void destroy_sync_objects(VulkanContext& ctx, FrameContext& frame_ctx) {
    for (VkSemaphore sem : frame_ctx.image_available_semaphores) {
        if (sem != VK_NULL_HANDLE) {
            vkDestroySemaphore(ctx.logical_device, sem, nullptr);
        }
    }
    frame_ctx.image_available_semaphores.clear();

    for (VkSemaphore sem : frame_ctx.render_finished_semaphores) {
        if (sem != VK_NULL_HANDLE) {
            vkDestroySemaphore(ctx.logical_device, sem, nullptr);
        }
    }
    frame_ctx.render_finished_semaphores.clear();

    for (VkFence fence : frame_ctx.in_flight_fences) {
        if (fence != VK_NULL_HANDLE) {
            vkDestroyFence(ctx.logical_device, fence, nullptr);
        }
    }
    frame_ctx.in_flight_fences.clear();
}

bool create_frame_context(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t count) {
    return create_command_buffers(ctx, frame_ctx, count) &&
        create_sync_objects(ctx, frame_ctx, count);
}

void destroy_frame_context(VulkanContext& ctx, FrameContext& frame_ctx) {
    destroy_sync_objects(ctx, frame_ctx);
    destroy_command_buffers(ctx, frame_ctx);
}

bool draw_frame(VulkanContext& ctx, SwapchainContext& swpch_ctx, FrameContext& frame_ctx, const Buffer& vertex_buffer, const Buffer& index_buffer, const DrawCallback& draw, const ExtraDrawCallback& extra_draw
) {
    VkFence current_fence = frame_ctx.in_flight_fences[frame_ctx.current_frame];
    VkSemaphore current_image_available = frame_ctx.image_available_semaphores[frame_ctx.current_frame];

    if (vkWaitForFences(ctx.logical_device, 1, &current_fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        return false;
    }

    uint32_t image_index = 0;

    VkResult result = vkAcquireNextImageKHR(ctx.logical_device, swpch_ctx.swapchain, UINT64_MAX, current_image_available, VK_NULL_HANDLE, &image_index);

    if (result != VK_SUCCESS) {
        return false;
    }

    if (vkResetFences(ctx.logical_device, 1, &current_fence) != VK_SUCCESS) {
        return false;
    }

    VkCommandBuffer command_buffer = frame_ctx.command_buffers[image_index];

    if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS) {
        return false;
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        std::cerr << "Echec du begin command buffer\n";
        return false;
    }

    VkClearValue clear_color{};
    clear_color.color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = swpch_ctx.render_pass;
    render_pass_info.framebuffer = swpch_ctx.framebuffers[image_index];
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = swpch_ctx.extent;
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swpch_ctx.extent.width);
    viewport.height = static_cast<float>(swpch_ctx.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swpch_ctx.extent;

    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    if (draw) {
        draw(command_buffer, ctx, vertex_buffer, index_buffer);
    }

    if (extra_draw) {
        extra_draw(command_buffer);
    }

    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        std::cerr << "Echec du end command buffer\n";
        return false;
    }

    VkSemaphore wait_semaphores[] = {
        current_image_available
    };

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore signal_semaphores[] = {
        frame_ctx.render_finished_semaphores[image_index]
    };

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(ctx.graphics_queue, 1, &submit_info, current_fence) != VK_SUCCESS) {
        std::cerr << "Echec de la soumission du command buffer\n";
        return false;
    }

    VkSwapchainKHR swapchains[] = {
        swpch_ctx.swapchain
    };

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    result = vkQueuePresentKHR(ctx.present_queue, &present_info);

    if (result != VK_SUCCESS) {
        std::cerr << "Echec de la presentation\n";
        return false;
    }

    frame_ctx.current_frame = (frame_ctx.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return true;
}