#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_device.hpp"

struct GLFWwindow;

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

struct Swapchain {
    VkSwapchainKHR swapchain;
    std::vector<VkImage> images;
    std::vector<VkImageView> image_views;
    VkFormat format;
    VkExtent2D extent;
};

SwapchainSupportDetails query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface);

Swapchain create_swapchain(VkPhysicalDevice physical_device, VkDevice device,
                             VkSurfaceKHR surface, GLFWwindow* window,
                             const QueueFamilyIndices& indices);

void destroy_swapchain(VkDevice device, const Swapchain& swapchain);
