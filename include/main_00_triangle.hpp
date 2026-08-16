#pragma once

#include "vulkan/vk_core.hpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex2D {
    glm::vec2 pos;
};

struct alignas(16) UBOColor {
    alignas(16) glm::vec3 color;
};

VkVertexInputBindingDescription get_vertex2d_binding_description();
std::vector<VkVertexInputAttributeDescription> get_vertex2d_attribute_descriptions();

void draw_triangle(VkCommandBuffer cmd, const GraphicsPipeline& pipeline, const Buffer& vb, const Buffer& ib, VkDescriptorSet descriptor_set);

void draw_ubo_color_ui(Buffer& uniform_buffer);

int main_00_triangle();