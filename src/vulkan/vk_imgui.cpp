#include "vulkan/vk_imgui.hpp"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <array>
#include <iostream>

ImGuiHandles init_imgui(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physical_device,
                          VkDevice device, uint32_t graphics_family_index, VkQueue graphics_queue,
                          VkRenderPass render_pass, uint32_t image_count) {
    ImGuiHandles handles{};

    std::array<VkDescriptorPoolSize, 1> pool_sizes = {{
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
    }};

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &handles.descriptor_pool) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du descriptor pool ImGui\n";
        return handles;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physical_device;
    init_info.Device = device;
    init_info.QueueFamily = graphics_family_index;
    init_info.Queue = graphics_queue;
    init_info.DescriptorPool = handles.descriptor_pool;
    init_info.RenderPass = render_pass;
    init_info.MinImageCount = image_count;
    init_info.ImageCount = image_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    return handles;
}

void shutdown_imgui(VkDevice device, const ImGuiHandles& handles) {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(device, handles.descriptor_pool, nullptr);
}

void begin_imgui_frame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");
    ImGui::Text("Hello depuis ImGui !");
    ImGui::End();

    ImGui::Render();
}

void render_imgui_frame(VkCommandBuffer command_buffer) {
    if (ImDrawData* draw_data = ImGui::GetDrawData()) {
        ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
    }
}