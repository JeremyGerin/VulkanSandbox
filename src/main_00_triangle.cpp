#include "main_00_triangle.hpp"
#include "vulkan/vk_core.hpp"

#include <cstdlib>
#include <GLFW/glfw3.h>

#define APP_NAME "VulkanSandbox"

int main_00_triangle() {
    CleanupStack cleanup;

    GLFWwindow* window = create_window(1200, 800, APP_NAME);
    if (!window) {
        return EXIT_FAILURE;
    }
    cleanup.push_back([&]{ glfwDestroyWindow(window); glfwTerminate(); });

    VkInstance instance = create_instance(APP_NAME);
    if (instance == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ vkDestroyInstance(instance, nullptr); });

    VkDebugUtilsMessengerEXT debug_messenger = setup_debug_messenger(instance);
    if (debug_messenger == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ destroy_debug_messenger(instance, debug_messenger); });

    VkSurfaceKHR surface = create_surface(instance, window);
    if (surface == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ vkDestroySurfaceKHR(instance, surface, nullptr); });

    VkPhysicalDevice physical_device = pick_physical_device(instance, surface);
    if (physical_device == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    // pas de destroy pour un VkPhysicalDevice, rien a empiler ici

    QueueFamilyIndices indices = find_queue_families(physical_device, surface);

    LogicalDevice logical_device = create_logical_device(physical_device, indices);
    if (logical_device.device == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ vkDestroyDevice(logical_device.device, nullptr); });

    Swapchain swapchain = create_swapchain(physical_device, logical_device.device, surface, window, indices);
    if (swapchain.swapchain == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ destroy_swapchain(logical_device.device, swapchain); });

    VkRenderPass render_pass = create_render_pass(logical_device.device, swapchain.format);
    if (render_pass == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ vkDestroyRenderPass(logical_device.device, render_pass, nullptr); });

    ImGuiHandles imgui_handles = init_imgui(window, instance, physical_device, logical_device.device, indices.graphics_family.value(), logical_device.graphics_queue, render_pass, static_cast<uint32_t>(swapchain.images.size()));
    cleanup.push_back([&]{ shutdown_imgui(logical_device.device, imgui_handles); });

    GraphicsPipeline graphics_pipeline = create_graphics_pipeline(logical_device.device, render_pass, swapchain.extent);
    if (graphics_pipeline.pipeline == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ destroy_graphics_pipeline(logical_device.device, graphics_pipeline); });

    std::vector<VkFramebuffer> framebuffers = create_framebuffers(logical_device.device, render_pass, swapchain);
    cleanup.push_back([&]{ destroy_framebuffers(logical_device.device, framebuffers); });

    VkCommandPool command_pool = create_command_pool(logical_device.device, indices.graphics_family.value());
    if (command_pool == VK_NULL_HANDLE) { unwind(cleanup); return EXIT_FAILURE; }
    cleanup.push_back([&]{ vkDestroyCommandPool(logical_device.device, command_pool, nullptr); });

    std::vector<VkCommandBuffer> command_buffers = create_command_buffers(logical_device.device, command_pool,
        static_cast<uint32_t>(framebuffers.size()));

    SyncObjects sync = create_sync_objects(logical_device.device, static_cast<uint32_t>(swapchain.images.size()));
    cleanup.push_back([&]{ destroy_sync_objects(logical_device.device, sync); });

    VmaAllocator allocator = create_allocator(instance, physical_device, logical_device.device);
    cleanup.push_back([&]{ destroy_allocator(allocator); });

    std::vector<Vertex2DColor> vertices = {
        { {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }
    };

    Buffer vertex_buffer = create_vertex_buffer(allocator, vertices);
    cleanup.push_back([&]{ destroy_buffer(allocator, vertex_buffer); });

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        begin_imgui_frame();
        draw_frame(logical_device.device, swapchain.swapchain, logical_device.graphics_queue, logical_device.present_queue,
                   command_buffers[0], render_pass, framebuffers, swapchain.extent, graphics_pipeline.pipeline,
                   sync, vertex_buffer.buffer, static_cast<uint32_t>(vertices.size()), render_imgui_frame);
    }

    vkDeviceWaitIdle(logical_device.device);
    unwind(cleanup);

    return EXIT_SUCCESS;
}
