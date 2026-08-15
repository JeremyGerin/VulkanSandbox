#pragma once
#include <vulkan/vulkan.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

struct VulkanContext;
struct SwapchainContext;
struct Buffer;

bool create_imgui(VulkanContext& ctx, SwapchainContext& swpch_ctx);
void destroy_imgui(VulkanContext& ctx);

void imgui_new_frame();
void imgui_end_frame();
void imgui_draw_callback(VkCommandBuffer cmd);

void draw_vertex_buffer_ui(const Buffer& vertex_buffer);