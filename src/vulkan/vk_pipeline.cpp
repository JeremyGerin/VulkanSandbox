#include "vulkan/vk_pipeline.hpp"
#include <iostream>
#include <fstream>

static std::vector<char> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << path << "\n";
        return {};
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(file_size));

    return buffer;
}

VkShaderModule create_shader_module(VkDevice device, const std::string& spv_path) {
    std::vector<char> code = read_file(spv_path);

    if (code.empty()) {
        return VK_NULL_HANDLE;
    }

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module = VK_NULL_HANDLE;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du shader module : " << spv_path << "\n";
        return VK_NULL_HANDLE;
    }

    return shader_module;
}

bool create_graphics_pipeline(VulkanContext& ctx, SwapchainContext& swpch_ctx, const PipelineInfo& info) {
    VkShaderModule vert_module = create_shader_module(ctx.logical_device, info.vertex_shader_path);
    VkShaderModule frag_module = create_shader_module(ctx.logical_device, info.fragment_shader_path);

    if (vert_module == VK_NULL_HANDLE || frag_module == VK_NULL_HANDLE) {
        std::cerr << "Echec du chargement des shaders\n";
        if (vert_module != VK_NULL_HANDLE) vkDestroyShaderModule(ctx.logical_device, vert_module, nullptr);
        if (frag_module != VK_NULL_HANDLE) vkDestroyShaderModule(ctx.logical_device, frag_module, nullptr);
        return false;
    }

    VkPipelineShaderStageCreateInfo vert_stage_info{};
    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = vert_module;
    vert_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_stage_info{};
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = frag_module;
    frag_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &info.binding_description;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(info.attribute_descriptions.size());
    vertex_input_info.pVertexAttributeDescriptions = info.attribute_descriptions.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(std::size(dynamic_states));
    dynamic_state.pDynamicStates = dynamic_states;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(ctx.logical_device, &layout_info, nullptr, &ctx.pipeline_layout) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du pipeline layout\n";
        vkDestroyShaderModule(ctx.logical_device, frag_module, nullptr);
        vkDestroyShaderModule(ctx.logical_device, vert_module, nullptr);
        return false;
    }

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = ctx.pipeline_layout;
    pipeline_info.renderPass = swpch_ctx.render_pass;
    pipeline_info.subpass = 0;

    bool ok = true;
    if (vkCreateGraphicsPipelines(ctx.logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &ctx.graphics_pipeline) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du pipeline graphique\n";
        vkDestroyPipelineLayout(ctx.logical_device, ctx.pipeline_layout, nullptr);
        ctx.pipeline_layout = VK_NULL_HANDLE;
        ctx.graphics_pipeline = VK_NULL_HANDLE;
        ok = false;
    }

    vkDestroyShaderModule(ctx.logical_device, frag_module, nullptr);
    vkDestroyShaderModule(ctx.logical_device, vert_module, nullptr);

    return ok;
}

void destroy_graphics_pipeline(VulkanContext& ctx, SwapchainContext& swpch_ctx) {
    (void)swpch_ctx;
    if (ctx.graphics_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(ctx.logical_device, ctx.graphics_pipeline, nullptr);
        ctx.graphics_pipeline = VK_NULL_HANDLE;
    }
    if (ctx.pipeline_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(ctx.logical_device, ctx.pipeline_layout, nullptr);
        ctx.pipeline_layout = VK_NULL_HANDLE;
    }
}