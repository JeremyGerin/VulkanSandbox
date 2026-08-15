#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct VulkanContext;

bool test_vulkan();

const std::vector<const char*> kValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

bool create_instance(VulkanContext& ctx, const char* app_name);
void destroy_instance(VulkanContext& ctx);

bool setup_debug_messenger(VulkanContext& ctx);
void destroy_debug_messenger(VulkanContext& ctx);