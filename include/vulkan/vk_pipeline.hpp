#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include <string>

VkShaderModule create_shader_module(VkDevice device, const std::string& spv_path);

struct Vertex2DColor {
    glm::vec2 pos;
    glm::vec3 color;
};

VkVertexInputBindingDescription get_vertex2d_color_binding_description();
std::array<VkVertexInputAttributeDescription, 2> get_vertex2d_color_attribute_descriptions();

struct GraphicsPipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

GraphicsPipeline create_graphics_pipeline(VkDevice device, VkRenderPass render_pass, VkExtent2D swapchain_extent);
void destroy_graphics_pipeline(VkDevice device, const GraphicsPipeline& pipeline);
