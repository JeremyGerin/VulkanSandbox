#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_context.hpp" 

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

struct SwapchainContext {
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> image_views;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = { 0, 0 };
    VkRenderPass render_pass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers;
};

SwapchainSupportDetails query_swapchain_support(VulkanContext& ctx);

bool create_swapchain(VulkanContext& ctx, SwapchainContext& swpch_ctx);
void destroy_swapchain(VulkanContext& ctx, SwapchainContext& swpch_ctx);

bool create_swapchain_context(VulkanContext& ctx, SwapchainContext& swpch_ctx);
void destroy_swapchain_context(VulkanContext& ctx, SwapchainContext& swpch_ctx);