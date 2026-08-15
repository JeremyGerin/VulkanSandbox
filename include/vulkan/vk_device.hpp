#pragma once

#include <vulkan/vulkan.h>

struct VulkanContext;
struct QueueFamilyIndices;

QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

bool pick_physical_device(VulkanContext& ctx);

bool create_logical_device(VulkanContext& ctx);
void destroy_logical_device(VulkanContext& ctx);