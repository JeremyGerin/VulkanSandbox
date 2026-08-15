#pragma once

#include "vulkan/vk_core.hpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex2DColor {
    glm::vec2 pos;
    glm::vec3 color;
};

VkVertexInputBindingDescription get_vertex2d_color_binding_description();
std::vector<VkVertexInputAttributeDescription> get_vertex2d_color_attribute_descriptions();

void draw_callback(VkCommandBuffer cmd, VulkanContext& ctx, const Buffer& vb, const Buffer& ib);

int main_00_triangle();