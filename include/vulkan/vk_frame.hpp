#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_sync.hpp"

void draw_frame(VkDevice device, VkSwapchainKHR swapchain, VkQueue graphics_queue, VkQueue present_queue, VkCommandBuffer command_buffer, VkRenderPass render_pass, const std::vector<VkFramebuffer>& framebuffers, VkExtent2D extent, VkPipeline pipeline, const SyncObjects& sync, VkBuffer vertex_buffer, uint32_t vertex_count);
