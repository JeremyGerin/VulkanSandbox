#pragma once

#include <vulkan/vulkan.h>
#include "vk_context.hpp"
#include "vk_swapchain.hpp"

bool create_render_pass(VulkanContext& ctx, SwapchainContext& swpch_ctx);
void destroy_render_pass(VulkanContext& ctx, SwapchainContext& swpch_ctx);
