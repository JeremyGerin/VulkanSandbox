#include "vulkan/vk_pipeline.hpp"
#include <iostream>
#include <fstream>
#include <vector>

std::vector<char> read_file(const std::string& path) {
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

VkVertexInputBindingDescription get_vertex2d_color_binding_description() {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex2DColor);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
}

std::array<VkVertexInputAttributeDescription, 2> get_vertex2d_color_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributes{};

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

GraphicsPipeline create_graphics_pipeline(VkDevice device, VkRenderPass render_pass, VkExtent2D swapchain_extent) {
    GraphicsPipeline result{};

    VkShaderModule vert_module = create_shader_module(device, std::string(SHADER_DIR) + "triangle.vert.spv");
    VkShaderModule frag_module = create_shader_module(device, std::string(SHADER_DIR) + "triangle.frag.spv");

    if (vert_module == VK_NULL_HANDLE || frag_module == VK_NULL_HANDLE) {
        std::cerr << "Echec du chargement des shaders\n";
        result.pipeline = VK_NULL_HANDLE;
        return result;
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

    VkVertexInputBindingDescription binding_description = get_vertex2d_color_binding_description();
    std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions = get_vertex2d_color_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain_extent.width);
    viewport.height = static_cast<float>(swapchain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain_extent;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

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

    if (vkCreatePipelineLayout(device, &layout_info, nullptr, &result.layout) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du pipeline layout\n";
        vkDestroyShaderModule(device, frag_module, nullptr);
        vkDestroyShaderModule(device, vert_module, nullptr);
        result.pipeline = VK_NULL_HANDLE;
        return result;
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
    pipeline_info.layout = result.layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &result.pipeline) != VK_SUCCESS) {
        std::cerr << "Echec de la creation du pipeline graphique\n";
        vkDestroyPipelineLayout(device, result.layout, nullptr);
        result.pipeline = VK_NULL_HANDLE;
    }

    vkDestroyShaderModule(device, frag_module, nullptr);
    vkDestroyShaderModule(device, vert_module, nullptr);

    return result;
}

void destroy_graphics_pipeline(VkDevice device, const GraphicsPipeline& pipeline) {
    vkDestroyPipeline(device, pipeline.pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline.layout, nullptr);
}
