#pragma once

#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;
    std::optional<uint32_t> compute_family;

    bool is_complete() const {
        return graphics_family.has_value()
            && present_family.has_value()
            && compute_family.has_value();
    }
};
QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);
VkPhysicalDevice pick_physical_device(VkInstance instance, VkSurfaceKHR surface);

struct LogicalDevice {
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue compute_queue;
};
LogicalDevice create_logical_device(VkPhysicalDevice physical_device, const QueueFamilyIndices& indices);