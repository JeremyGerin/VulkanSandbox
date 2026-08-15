#include "vulkan/vk_imgui.hpp"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_swapchain.hpp"
#include "vulkan/vk_buffer.hpp"

#include <iostream>

bool create_imgui(VulkanContext& ctx, SwapchainContext& swpch_ctx) {
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16 }
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 16;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(ctx.logical_device, &pool_info, nullptr, &ctx.imgui_descriptor_pool) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du descriptor pool ImGui\n";
        return false;
    }

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(ctx.window, true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance       = ctx.instance;
    init_info.PhysicalDevice = ctx.physical_device;
    init_info.Device         = ctx.logical_device;
    init_info.QueueFamily    = ctx.queue_family_indices.graphics.value();
    init_info.Queue          = ctx.graphics_queue;
    init_info.DescriptorPool = ctx.imgui_descriptor_pool;
    init_info.RenderPass     = swpch_ctx.render_pass;
    init_info.MinImageCount  = 2;
    init_info.ImageCount     = static_cast<uint32_t>(swpch_ctx.images.size());
    init_info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

    if (!ImGui_ImplVulkan_Init(&init_info)) {
        std::cerr << "Echec de l'initialisation du backend Vulkan d'ImGui\n";
        destroy_imgui(ctx);
        return false;
    }

    return true;
}

void destroy_imgui(VulkanContext& ctx) {
    if (ctx.imgui_descriptor_pool == VK_NULL_HANDLE) {
        return;
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(ctx.logical_device, ctx.imgui_descriptor_pool, nullptr);
    ctx.imgui_descriptor_pool = VK_NULL_HANDLE;
}

void imgui_new_frame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_end_frame() {
    ImGui::Render();
}

void imgui_draw_callback(VkCommandBuffer cmd) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void draw_vertex_buffer_ui(const Buffer& vertex_buffer) {
    ImGui::Begin("Vertex buffer");
        ImGui::Text("Nombre de vertex : %u", vertex_buffer.count);
        ImGui::Text("Taille totale : %llu octets", static_cast<unsigned long long>(vertex_buffer.size_bytes));
    ImGui::End();
}
