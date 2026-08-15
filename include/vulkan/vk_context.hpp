#pragma once 

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <optional>

struct GLFWwindow;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;
    std::optional<uint32_t> compute;

    bool is_complete() const {
        return graphics.has_value()
            && present.has_value()
            && compute.has_value();
    }
};

struct VulkanContext {
    GLFWwindow* window = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    QueueFamilyIndices queue_family_indices;
    VkDevice logical_device = VK_NULL_HANDLE;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    VkQueue compute_queue = VK_NULL_HANDLE;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    VkPipeline graphics_pipeline = VK_NULL_HANDLE;
    VmaAllocator allocator = nullptr;
    VkDescriptorPool imgui_descriptor_pool = VK_NULL_HANDLE;
};

bool create_vulkan_context(VulkanContext& ctx, int width, int height, const char* title);

void destroy_vulkan_context(VulkanContext& ctx);