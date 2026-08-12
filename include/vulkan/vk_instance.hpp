#pragma once

#include <vulkan/vulkan.h>
#include <vector>

bool test_vulkan();

const std::vector<const char*> kValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

VkInstance create_instance(const char* app_name);

VkDebugUtilsMessengerEXT setup_debug_messenger(VkInstance instance);
void destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger);