#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "vk_context.hpp"
#include "vk_swapchain.hpp"

VkShaderModule create_shader_module(VkDevice device, const std::string& spv_path);

struct PipelineInfo {
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    VkVertexInputBindingDescription binding_description;
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
};

bool create_graphics_pipeline(VulkanContext& ctx, SwapchainContext& swpch_ctx, const PipelineInfo& info);
void destroy_graphics_pipeline(VulkanContext& ctx, SwapchainContext& swpch_ctx);