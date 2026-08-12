#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

VkCommandPool create_command_pool(VkDevice device, uint32_t graphics_family_index);

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count);

void record_command_buffer(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkExtent2D extent, VkPipeline pipeline, VkBuffer vertex_buffer, uint32_t vertex_count, const std::function<void(VkCommandBuffer)>& extra_draw = nullptr);
