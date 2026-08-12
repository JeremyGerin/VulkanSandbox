#include "vulkan/vk_device.hpp"
#include "vulkan/vk_instance.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <set>

const std::vector<const char*> kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.compute_family = i;
        }

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }

        if (indices.is_complete()) {
            break;
        }
    }

    return indices;
}

bool check_device_extension_support(VkPhysicalDevice device) {
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available.data());

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

bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = find_queue_families(device, surface);
    bool extensions_supported = check_device_extension_support(device);

    return indices.is_complete() && extensions_supported;
}

VkPhysicalDevice pick_physical_device(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
        std::cerr << "Aucun GPU compatible Vulkan trouve\n";
        return VK_NULL_HANDLE;
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    VkPhysicalDevice best_device = VK_NULL_HANDLE;
    int best_score = -1;

    for (VkPhysicalDevice device : devices) {
        if (!is_device_suitable(device, surface)) {
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

    return best_device;
}

LogicalDevice create_logical_device(VkPhysicalDevice physical_device, const QueueFamilyIndices& indices) {
    std::set<uint32_t> unique_families = {
        indices.graphics_family.value(),
        indices.present_family.value(),
        indices.compute_family.value()
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

    LogicalDevice result{};
    if (vkCreateDevice(physical_device, &create_info, nullptr, &result.device) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du logical device\n";
        result.device = VK_NULL_HANDLE;
        return result;
    }

    vkGetDeviceQueue(result.device, indices.graphics_family.value(), 0, &result.graphics_queue);
    vkGetDeviceQueue(result.device, indices.present_family.value(), 0, &result.present_queue);
    vkGetDeviceQueue(result.device, indices.compute_family.value(), 0, &result.compute_queue);

    return result;
}