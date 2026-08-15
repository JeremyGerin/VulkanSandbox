#include "vulkan/vk_device.hpp"

#include "vulkan/vk_context.hpp"
#include "vulkan/vk_instance.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <set>

const std::vector<const char*> kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics = i;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.compute = i;
        }

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        if (present_support) {
            indices.present = i;
        }

        if (indices.is_complete()) {
            break;
        }
    }

    return indices;
}

bool check_device_extension_support(VkPhysicalDevice physical_device) {
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available(extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available.data());

    for (const char* required : kDeviceExtensions) {
        bool found = false;
        for (const auto& ext : available) {
            if (std::strcmp(required, ext.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool is_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = find_queue_families(physical_device, surface);
    bool extensions_supported = check_device_extension_support(physical_device);

    return indices.is_complete() && extensions_supported;
}

bool pick_physical_device(VulkanContext& ctx) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(ctx.instance, &device_count, nullptr);

    if (device_count == 0) {
        std::cerr << "Aucun GPU compatible Vulkan trouve\n";
        return false;
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(ctx.instance, &device_count, devices.data());

    VkPhysicalDevice best_device = VK_NULL_HANDLE;
    int best_score = -1;

    for (VkPhysicalDevice device : devices) {
        if (!is_device_suitable(device, ctx.surface)) {
            continue;
        }

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        int score = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }
        score += static_cast<int>(properties.limits.maxImageDimension2D);

        if (score > best_score) {
            best_score = score;
            best_device = device;
        }
    }

    if (best_device == VK_NULL_HANDLE) {
        std::cerr << "Aucun GPU adapte trouve\n";
    }

    ctx.physical_device = best_device;

    return true;
}

bool create_logical_device(VulkanContext& ctx) {
    ctx.queue_family_indices = find_queue_families(ctx.physical_device, ctx.surface);

    std::set<uint32_t> unique_families = {
        ctx.queue_family_indices.graphics.value(),
        ctx.queue_family_indices.present.value(),
        ctx.queue_family_indices.compute.value()
    };

    float queue_priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    for (uint32_t family : unique_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
    create_info.ppEnabledExtensionNames = kDeviceExtensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
    create_info.ppEnabledLayerNames = kValidationLayers.data();

    if (vkCreateDevice(ctx.physical_device, &create_info, nullptr, &ctx.logical_device) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du logical device\n";
        return false;
    }

    vkGetDeviceQueue(ctx.logical_device, ctx.queue_family_indices.graphics.value(), 0, &ctx.graphics_queue);
    vkGetDeviceQueue(ctx.logical_device, ctx.queue_family_indices.present.value(), 0, &ctx.present_queue);
    vkGetDeviceQueue(ctx.logical_device, ctx.queue_family_indices.compute.value(), 0, &ctx.compute_queue);

    return true;
}

void destroy_logical_device(VulkanContext& ctx) {
    if (ctx.logical_device != VK_NULL_HANDLE) {
        return;
    }

    vkDestroyDevice(ctx.logical_device, nullptr);
    ctx.logical_device = VK_NULL_HANDLE;
}
