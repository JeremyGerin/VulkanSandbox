#pragma once

#include <vulkan/vulkan.h>
struct GLFWwindow;

struct ImGuiHandles {
    VkDescriptorPool descriptor_pool;
};

ImGuiHandles init_imgui(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physical_device,
                          VkDevice device, uint32_t graphics_family_index, VkQueue graphics_queue,
                          VkRenderPass render_pass, uint32_t image_count);

void shutdown_imgui(VkDevice device, const ImGuiHandles& handles);

void begin_imgui_frame();
void render_imgui_frame(VkCommandBuffer command_buffer);