#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_swapchain.hpp"
#include "vulkan/vk_buffer.hpp"
#include "vulkan/vk_pipeline.hpp"

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

struct FrameContext {
    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> render_finished_semaphores;
    std::vector<VkFence> in_flight_fences;
    uint32_t current_frame = 0;
};

bool create_command_buffers(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t count);
void destroy_command_buffers(VulkanContext& ctx, FrameContext& frame_ctx);

bool create_sync_objects(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t swapchain_image_count);
void destroy_sync_objects(VulkanContext& ctx, FrameContext& frame_ctx);

bool create_frame_context(VulkanContext& ctx, FrameContext& frame_ctx, uint32_t count);
void destroy_frame_context(VulkanContext& ctx, FrameContext& frame_ctx);

using DrawCallback = std::function<void(VkCommandBuffer)>;

bool draw_frame(VulkanContext& ctx, SwapchainContext& swpch_ctx, FrameContext& frame_ctx, const DrawCallback& draw, const DrawCallback& extra_draw = {});