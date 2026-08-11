#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <vk_mem_alloc.h>
#include <array>

bool test_vulkan();

struct GLFWwindow;
GLFWwindow* create_window(int width, int height, const char* title);

VkInstance create_instance(const char* app_name);

VkDebugUtilsMessengerEXT setup_debug_messenger(VkInstance instance);
void destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger);

VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow* window);

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

VkRenderPass create_render_pass(VkDevice device, VkFormat swapchain_format);

VkShaderModule create_shader_module(VkDevice device, const std::string& spv_path);

struct GraphicsPipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

GraphicsPipeline create_graphics_pipeline(VkDevice device, VkRenderPass render_pass, VkExtent2D swapchain_extent);
void destroy_graphics_pipeline(VkDevice device, const GraphicsPipeline& pipeline);

std::vector<VkFramebuffer> create_framebuffers(VkDevice device, VkRenderPass render_pass, const Swapchain& swapchain);
void destroy_framebuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers);

VkCommandPool create_command_pool(VkDevice device, uint32_t graphics_family_index);
std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count);

void record_command_buffer(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkExtent2D extent, VkPipeline pipeline, VkBuffer vertex_buffer, uint32_t vertex_count);

struct SyncObjects {
    VkSemaphore image_available_semaphore;
    std::vector<VkSemaphore> render_finished_semaphores;
    VkFence in_flight_fence;
};

SyncObjects create_sync_objects(VkDevice device, uint32_t swapchain_image_count);
void destroy_sync_objects(VkDevice device, const SyncObjects& sync);

void draw_frame(VkDevice device, VkSwapchainKHR swapchain, VkQueue graphics_queue, VkQueue present_queue, VkCommandBuffer command_buffer, VkRenderPass render_pass, const std::vector<VkFramebuffer>& framebuffers, VkExtent2D extent, VkPipeline pipeline, const SyncObjects& sync, VkBuffer vertex_buffer, uint32_t vertex_count);

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

VkVertexInputBindingDescription get_vertex_binding_description();
std::array<VkVertexInputAttributeDescription, 2> get_vertex_attribute_descriptions();

struct Buffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};

VmaAllocator create_allocator(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device);
void destroy_allocator(VmaAllocator allocator);

Buffer create_buffer(VmaAllocator allocator, VkDeviceSize size, 
                       VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
void destroy_buffer(VmaAllocator allocator, const Buffer& buffer);

Buffer create_vertex_buffer(VmaAllocator allocator, const std::vector<Vertex>& vertices);

