#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_swapchain.hpp"
#include "vulkan/vk_buffer.hpp"

struct FrameContext {
    std::vector<VkCommandBuffer> command_buffers;
    VkSemaphore image_available_semaphore = VK_NULL_HANDLE;
    std::vector<VkSemaphore> render_finished_semaphores;
    VkFence in_flight_fence = VK_NULL_HANDLE;
};

bool create_command_buffers(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t count);
void destroy_command_buffers(VulkanContext& ctx, FrameContext& frame_ctx);

bool create_sync_objects(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t swapchain_image_count);
void destroy_sync_objects(VulkanContext& ctx, FrameContext& frame_ctx);

bool create_frame_context(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t count);
void destroy_frame_context(VulkanContext& ctx, FrameContext& frame_ctx);

using DrawCallback = std::function<void(VkCommandBuffer, VulkanContext&, const Buffer&, const Buffer&)>;
using ExtraDrawCallback = std::function<void(VkCommandBuffer)>;

bool draw_frame(VulkanContext& ctx, SwapchainContext& swpch_ctx, FrameContext& frame_ctx, const Buffer& vertex_buffer, const Buffer& index_buffer, const DrawCallback& draw, const ExtraDrawCallback& extra_draw = {});