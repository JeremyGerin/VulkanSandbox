#include "main_00_triangle.hpp"

#include <cstddef> 

VkVertexInputBindingDescription get_vertex2d_color_binding_description() {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex2DColor);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
}

std::vector<VkVertexInputAttributeDescription> get_vertex2d_color_attribute_descriptions() {
    std::vector<VkVertexInputAttributeDescription> attributes(2);

    attributes[0].binding = 0;
    attributes[0].location = 0;
    attributes[0].format = VK_FORMAT_R32G32_SFLOAT; // vec2
    attributes[0].offset = offsetof(Vertex2DColor, pos);

    attributes[1].binding = 0;
    attributes[1].location = 1;
    attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3
    attributes[1].offset = offsetof(Vertex2DColor, color);

    return attributes;
}

void draw_triangle(VkCommandBuffer cmd, const GraphicsPipeline& pipeline, const Buffer& vb, const Buffer& ib) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

    VkBuffer buffers[] = { vb.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);

    vkCmdBindIndexBuffer(cmd, ib.buffer, 0, ib.index_type);
    vkCmdDrawIndexed(cmd, ib.count, 1, 0, 0, 0);
}

int main_00_triangle() {
    VulkanContext ctx;
    SwapchainContext swapchain_ctx;
    FrameContext frame_ctx;
    Buffer vertex_buffer;
    Buffer index_buffer;
    GraphicsPipeline graphics_pipeline;

    std::vector<Vertex2DColor> vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } }, 
        { {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } }, 
        { {  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }, 
        { {  0.5f, 0.5f }, { 1.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } },
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } }
    };

    std::vector<uint32_t> indices = {
        3, 4, 5,
        0, 1, 2
    };

    PipelineInfo pipeline_info{};
    pipeline_info.vertex_shader_path = std::string(SHADER_DIR) + "triangle.vert.spv";
    pipeline_info.fragment_shader_path = std::string(SHADER_DIR) + "triangle.frag.spv";
    pipeline_info.binding_description = get_vertex2d_color_binding_description();
    pipeline_info.attribute_descriptions = get_vertex2d_color_attribute_descriptions();

    bool ok = create_vulkan_context(ctx, 1200, 800, "VulkanSandbox") &&
        create_swapchain_context(ctx, swapchain_ctx) &&
        create_graphics_pipeline(ctx, swapchain_ctx, pipeline_info, graphics_pipeline) &&
        create_imgui(ctx, swapchain_ctx) &&
        create_frame_context(ctx, frame_ctx, static_cast<uint32_t>(swapchain_ctx.framebuffers.size())) &&
        create_vertex_buffer(ctx, vertices, vertex_buffer) &&
        create_index_buffer(ctx, indices, VK_INDEX_TYPE_UINT32, index_buffer);

    auto draw = [&](VkCommandBuffer cmd) {
        draw_triangle(cmd, graphics_pipeline, vertex_buffer, index_buffer);
    };

    if (ok) {
        while (!glfwWindowShouldClose(ctx.window)) {
            glfwPollEvents();

            imgui_new_frame();
            draw_vertex_buffer_ui(vertex_buffer);
            imgui_end_frame();

            draw_frame(ctx, swapchain_ctx, frame_ctx, draw, imgui_draw_callback);
        }

        vkDeviceWaitIdle(ctx.logical_device);
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
