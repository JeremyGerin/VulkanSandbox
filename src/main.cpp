#include "../include/vk_core.hpp"
#include <cstdlib>
#include <GLFW/glfw3.h>

#define APP_NAME "VulkanSandbox"

int main() {
    GLFWwindow* window = create_window(1200, 800, APP_NAME);
    if (!window) {
        return EXIT_FAILURE;
    }

    VkInstance instance = create_instance(APP_NAME);
    if (instance == VK_NULL_HANDLE) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    VkDebugUtilsMessengerEXT debug_messenger = setup_debug_messenger(instance);
    if (debug_messenger == VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    VkSurfaceKHR surface = create_surface(instance, window);
    if (surface == VK_NULL_HANDLE) {
        destroy_debug_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    VkPhysicalDevice physical_device = pick_physical_device(instance, surface);
    if (physical_device == VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroy_debug_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    LogicalDevice logical_device = create_logical_device(physical_device, find_queue_families(physical_device, surface));
    if (logical_device.device == VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroy_debug_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Swapchain swapchain = create_swapchain(physical_device, logical_device.device, surface, window, find_queue_families(physical_device, surface));
    if (swapchain.swapchain == VK_NULL_HANDLE) {
        vkDestroyDevice(logical_device.device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroy_debug_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    VkRenderPass render_pass = create_render_pass(logical_device.device, swapchain.format);
    if (render_pass == VK_NULL_HANDLE) {
        destroy_swapchain(logical_device.device, swapchain);
        vkDestroyDevice(logical_device.device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroy_debug_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    GraphicsPipeline graphics_pipeline = create_graphics_pipeline(logical_device.device, render_pass, swapchain.extent);
    if (graphics_pipeline.pipeline == VK_NULL_HANDLE) {
        vkDestroyRenderPass(logical_device.device, render_pass, nullptr);
        destroy_swapchain(logical_device.device, swapchain);
        vkDestroyDevice(logical_device.device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        destroy_debug_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::vector<VkFramebuffer> framebuffers = create_framebuffers(logical_device.device, render_pass, swapchain);

    VkCommandPool command_pool = create_command_pool(logical_device.device, 
    find_queue_families(physical_device, surface).graphics_family.value());
    if (command_pool == VK_NULL_HANDLE) { /* cleanup + return */ }

    std::vector<VkCommandBuffer> command_buffers = create_command_buffers(logical_device.device, command_pool, 
        static_cast<uint32_t>(framebuffers.size()));

    SyncObjects sync = create_sync_objects(logical_device.device, static_cast<uint32_t>(swapchain.images.size()));

    VmaAllocator allocator = create_allocator(instance, physical_device, logical_device.device);

    std::vector<Vertex> vertices = {
        { {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }
    };

    Buffer vertex_buffer = create_vertex_buffer(allocator, vertices);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        draw_frame(logical_device.device, swapchain.swapchain, logical_device.graphics_queue, logical_device.present_queue, command_buffers[0], render_pass, framebuffers, swapchain.extent, graphics_pipeline.pipeline, sync, vertex_buffer.buffer, static_cast<uint32_t>(vertices.size()));
    }

    vkDeviceWaitIdle(logical_device.device);

    destroy_buffer(allocator, vertex_buffer);
    destroy_allocator(allocator);
    destroy_sync_objects(logical_device.device, sync);
    vkDestroyCommandPool(logical_device.device, command_pool, nullptr);
    destroy_framebuffers(logical_device.device, framebuffers);
    destroy_graphics_pipeline(logical_device.device, graphics_pipeline);
    vkDestroyRenderPass(logical_device.device, render_pass, nullptr);
    destroy_swapchain(logical_device.device, swapchain);
    vkDestroyDevice(logical_device.device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    destroy_debug_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}