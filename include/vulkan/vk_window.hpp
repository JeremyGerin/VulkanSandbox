#pragma once

struct VulkanContext;

bool create_window(VulkanContext& ctx, int width, int height, const char* title);
void destroy_window(VulkanContext& ctx);

bool create_surface(VulkanContext& ctx);
void destroy_surface(VulkanContext& ctx);