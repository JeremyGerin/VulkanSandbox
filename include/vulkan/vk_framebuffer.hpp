#pragma once

#include <vulkan/vulkan.h>
#include "vk_context.hpp"
#include "vk_swapchain.hpp"

bool create_framebuffers(VulkanContext& ctx, SwapchainContext& swpch_ctx);
void destroy_framebuffers(VulkanContext& ctx, SwapchainContext& swpch_ctx);