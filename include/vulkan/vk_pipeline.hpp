#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "vk_context.hpp"
#include "vk_swapchain.hpp"

VkShaderModule create_shader_module(VkDevice device, const std::string& spv_path);

struct GraphicsPipeline {
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
};

struct PipelineInfo {
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    VkVertexInputBindingDescription binding_description;
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
    std::vector<VkDescriptorSetLayoutBinding> descriptor_bindings;
};

bool create_graphics_pipeline(VulkanContext& ctx, SwapchainContext& swpch_ctx, const PipelineInfo& info, GraphicsPipeline& out_pipeline);
void destroy_graphics_pipeline(VulkanContext& ctx, GraphicsPipeline& pipeline);
