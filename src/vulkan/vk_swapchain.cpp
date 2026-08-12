#include "vulkan/vk_swapchain.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

SwapchainSupportDetails query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available) {
    for (const auto& format : available) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return available[0]; // fallback : on prend le premier disponible
}

VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available) {
    for (const auto& mode : available) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode; // triple buffering, faible latence
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR; // toujours disponible, equivalent vsync classique
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actual_extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actual_extent.width = std::clamp(actual_extent.width,
        capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height,
        capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
}

Swapchain create_swapchain(VkPhysicalDevice physical_device, VkDevice device,
                             VkSurfaceKHR surface, GLFWwindow* window,
                             const QueueFamilyIndices& indices) {
    SwapchainSupportDetails support = query_swapchain_support(physical_device, surface);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(support.present_modes);
    VkExtent2D extent = choose_swap_extent(support.capabilities, window);

    uint32_t image_count = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount) {
        image_count = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    Swapchain result{};
    if (vkCreateSwapchainKHR(device, &create_info, nullptr, &result.swapchain) != VK_SUCCESS) {
        std::cerr << "Echec de la creation de la swapchain\n";
        result.swapchain = VK_NULL_HANDLE;
        return result;
    }

    vkGetSwapchainImagesKHR(device, result.swapchain, &image_count, nullptr);
    result.images.resize(image_count);
    vkGetSwapchainImagesKHR(device, result.swapchain, &image_count, result.images.data());

    result.format = surface_format.format;
    result.extent = extent;

    result.image_views.resize(result.images.size());
    for (size_t i = 0; i < result.images.size(); ++i) {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = result.images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = result.format;
        view_info.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                   VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &view_info, nullptr, &result.image_views[i]) != VK_SUCCESS) {
            std::cerr << "Echec de la creation d'une image view\n";
        }
    }

    return result;
}

void destroy_swapchain(VkDevice device, const Swapchain& swapchain) {
    for (VkImageView view : swapchain.image_views) {
        vkDestroyImageView(device, view, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);
}
