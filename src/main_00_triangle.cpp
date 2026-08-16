#include "main_00_triangle.hpp"

#include <cstddef> 
#include <glm/gtc/type_ptr.hpp>

VkVertexInputBindingDescription get_vertex2d_binding_description() {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex2D);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
}

std::vector<VkVertexInputAttributeDescription> get_vertex2d_attribute_descriptions() {
    std::vector<VkVertexInputAttributeDescription> attributes(1);

    attributes[0].binding = 0;
    attributes[0].location = 0;
    attributes[0].format = VK_FORMAT_R32G32_SFLOAT; // vec2
    attributes[0].offset = offsetof(Vertex2D, pos);

    return attributes;
}

void draw_triangle(VkCommandBuffer cmd, const GraphicsPipeline& pipeline, const Buffer& vb, const Buffer& ib, VkDescriptorSet descriptor_set) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.layout, 0, 1, &descriptor_set, 0, nullptr);

    VkBuffer buffers[] = { vb.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);

    vkCmdBindIndexBuffer(cmd, ib.buffer, 0, ib.index_type);
    vkCmdDrawIndexed(cmd, ib.count, 1, 0, 0, 0);
}

void draw_ubo_color_ui(Buffer& uniform_buffer) {
    auto* ubo = static_cast<UBOColor*>(uniform_buffer.mapped_data);

    ImGui::Begin("Uniform buffer");
    ImGui::ColorEdit3("Couleur", glm::value_ptr(ubo->color));
    ImGui::End();
}

int main_00_triangle() {
    VulkanContext ctx;
    SwapchainContext swapchain_ctx;
    FrameContext frame_ctx;
    Buffer vertex_buffer;
    Buffer index_buffer;
    GraphicsPipeline graphics_pipeline;
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    std::vector<Buffer> uniform_buffers(MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSet> descriptor_sets;

    std::vector<Vertex2D> vertices = {
        { { -0.5f, -0.5f } },
        { {  0.5f, -0.5f } },
        { {  0.5f,  0.5f } },
        { {  0.5f,  0.5f } },
        { { -0.5f,  0.5f } },
        { { -0.5f, -0.5f } }
    };

    std::vector<uint32_t> indices = {
        3, 4, 5,
        0, 1, 2
    };

    VkDescriptorSetLayoutBinding ubo_binding{};
    ubo_binding.binding = 0;
    ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_binding.descriptorCount = 1;
    ubo_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    PipelineInfo pipeline_info{};
    pipeline_info.vertex_shader_path = std::string(SHADER_DIR) + "triangle.vert.spv";
    pipeline_info.fragment_shader_path = std::string(SHADER_DIR) + "triangle.frag.spv";
    pipeline_info.binding_description = get_vertex2d_binding_description();
    pipeline_info.attribute_descriptions = get_vertex2d_attribute_descriptions();
    pipeline_info.descriptor_bindings = { ubo_binding };

    bool ok = create_vulkan_context(ctx, 1200, 800, "VulkanSandbox") &&
        create_swapchain_context(ctx, swapchain_ctx) &&
        create_graphics_pipeline(ctx, swapchain_ctx, pipeline_info, graphics_pipeline) &&
        create_imgui(ctx, swapchain_ctx) &&
        create_frame_context(ctx, frame_ctx, static_cast<uint32_t>(swapchain_ctx.framebuffers.size())) &&
        create_vertex_buffer(ctx, vertices, vertex_buffer) &&
        create_index_buffer(ctx, indices, VK_INDEX_TYPE_UINT32, index_buffer);

    for (uint32_t i = 0; ok && i < MAX_FRAMES_IN_FLIGHT; ++i) {
        ok = create_uniform_buffer(ctx, sizeof(UBOColor), uniform_buffers[i]);
    }

    if (ok) {
        VkDescriptorPoolSize pool_size{};
        pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        ok = create_descriptor_pool(ctx, { pool_size }, MAX_FRAMES_IN_FLIGHT, descriptor_pool) && 
            allocate_descriptor_sets(ctx, descriptor_pool, graphics_pipeline.descriptor_set_layout, MAX_FRAMES_IN_FLIGHT, descriptor_sets);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            update_uniform_descriptor_set(ctx, descriptor_sets[i], 0, uniform_buffers[i], sizeof(UBOColor));
        }
    }

    auto draw = [&](VkCommandBuffer cmd) {
        draw_triangle(cmd, graphics_pipeline, vertex_buffer, index_buffer, descriptor_sets[frame_ctx.current_frame]);
    };

    if (ok) {
        while (!glfwWindowShouldClose(ctx.window)) {
            glfwPollEvents();

            imgui_new_frame();
            draw_vertex_buffer_ui(vertex_buffer);
            draw_ubo_color_ui(uniform_buffers[frame_ctx.current_frame]);
            imgui_end_frame();

            draw_frame(ctx, swapchain_ctx, frame_ctx, draw, imgui_draw_callback);
        }

        vkDeviceWaitIdle(ctx.logical_device);
    }
    
    destroy_descriptor_pool(ctx, descriptor_pool);
    for (auto& ubuf : uniform_buffers) {
        destroy_buffer(ctx, ubuf);
    }
    destroy_buffer(ctx, index_buffer);
    destroy_buffer(ctx, vertex_buffer);
    destroy_frame_context(ctx, frame_ctx);
    destroy_imgui(ctx);
    destroy_graphics_pipeline(ctx, graphics_pipeline);
    destroy_swapchain_context(ctx, swapchain_ctx);
    destroy_vulkan_context(ctx);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
