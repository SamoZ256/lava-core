#include "lvcore/vulkan/graphics_pipeline.hpp"

#include "lvcore/vulkan/device.hpp"
#include "lvcore/vulkan/swap_chain.hpp"
#include "lvcore/vulkan/descriptor_set.hpp"

namespace lv {

namespace vulkan {

GraphicsPipeline::GraphicsPipeline(internal::GraphicsPipelineCreateInfo createInfo) {
    if (createInfo.renderPass == nullptr)
        throw std::runtime_error("You must specify a valid render pass");
    
    vertexShaderModule = static_cast<ShaderModule*>(createInfo.vertexShaderModule);
    fragmentShaderModule = static_cast<ShaderModule*>(createInfo.fragmentShaderModule);
    _pipelineLayout = static_cast<PipelineLayout*>(createInfo.pipelineLayout);
    
    compile(createInfo);
}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipeline(g_vulkan_device->device(), _graphicsPipeline, nullptr);
}

void GraphicsPipeline::compile(internal::GraphicsPipelineCreateInfo& createInfo) {
    CAST_FROM_INTERNAL_NAMED(createInfo.renderPass, RenderPass, renderPass);
    CAST_FROM_INTERNAL_NAMED(createInfo.vertexDescriptor, VertexDescriptor, vertexDescriptor);

    VkCullModeFlags vkCullMode;
    GET_VK_CULL_MODE(createInfo.cullMode, vkCullMode);
    VkCompareOp vkCompareOp;
    GET_VK_COMPARE_OP(createInfo.depthOp, vkCompareOp);

    PipelineConfigInfo configInfo;

    //Input assembly
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    //Viewport
    configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.pViewports = nullptr;
    configInfo.viewportInfo.scissorCount = 1;
    configInfo.viewportInfo.pScissors = nullptr;

    //Rasterization
    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = vkCullMode;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    //Multisample
    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
    configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    //Color blending (attachment)
    configInfo.colorBlendAttachments.resize(createInfo.colorBlendAttachments.size());
    for (uint8_t i = 0; i < createInfo.colorBlendAttachments.size(); i++) {
        VkBlendFactor sourceRGBVKBlendFactor;
        GET_VK_BLEND_FACTOR(createInfo.colorBlendAttachments[i].srcRgbBlendFactor, sourceRGBVKBlendFactor);
        VkBlendFactor destinationRGBVKBlendFactor;
        GET_VK_BLEND_FACTOR(createInfo.colorBlendAttachments[i].dstRgbBlendFactor, destinationRGBVKBlendFactor);
        VkBlendOp rgbVKBlendOperation;
        GET_VK_BLEND_OPERATION(createInfo.colorBlendAttachments[i].rgbBlendOp, rgbVKBlendOperation);

        VkBlendFactor sourceAlphaVKBlendFactor;
        GET_VK_BLEND_FACTOR(createInfo.colorBlendAttachments[i].srcAlphaBlendFactor, sourceAlphaVKBlendFactor);
        VkBlendFactor destinationAlphaVKBlendFactor;
        GET_VK_BLEND_FACTOR(createInfo.colorBlendAttachments[i].dstAlphaBlendFactor, destinationAlphaVKBlendFactor);
        VkBlendOp alphaVKBlendOperation;
        GET_VK_BLEND_OPERATION(createInfo.colorBlendAttachments[i].alphaBlendOp, alphaVKBlendOperation);
        
        //uint8_t index = i;//colorBlendAttachments[i].index;
        configInfo.colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        configInfo.colorBlendAttachments[i].blendEnable = (VkBool32)createInfo.colorBlendAttachments[i].blendEnable;
        configInfo.colorBlendAttachments[i].srcColorBlendFactor = sourceRGBVKBlendFactor;
        configInfo.colorBlendAttachments[i].dstColorBlendFactor = destinationRGBVKBlendFactor;
        configInfo.colorBlendAttachments[i].colorBlendOp = rgbVKBlendOperation;
        configInfo.colorBlendAttachments[i].srcAlphaBlendFactor = sourceAlphaVKBlendFactor;
        configInfo.colorBlendAttachments[i].dstAlphaBlendFactor = destinationAlphaVKBlendFactor;
        configInfo.colorBlendAttachments[i].alphaBlendOp = alphaVKBlendOperation;
    }

    //Color blending (info)
    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    configInfo.colorBlendInfo.attachmentCount = configInfo.colorBlendAttachments.size();
    configInfo.colorBlendInfo.pAttachments = configInfo.colorBlendAttachments.data();
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    //Depth and stencil
    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = (VkBool32)createInfo.depthTestEnable;
    configInfo.depthStencilInfo.depthWriteEnable = (createInfo.depthTestEnable ? (VkBool32)createInfo.depthWriteEnable : VK_FALSE);
    configInfo.depthStencilInfo.depthCompareOp = vkCompareOp;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.front = {};  // Optional
    configInfo.depthStencilInfo.back = {};   // Optional

    //Dynamic state
    configInfo.dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStates.data();
    configInfo.dynamicStateInfo.dynamicStateCount = (uint32_t)configInfo.dynamicStates.size();
    configInfo.dynamicStateInfo.flags = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (createInfo.vertexDescriptor != nullptr) {
        vertexInputInfo.vertexBindingDescriptionCount = vertexDescriptor->bindingDescriptionCount();
        vertexInputInfo.pVertexBindingDescriptions = vertexDescriptor->bindingDescriptionsData();
        vertexInputInfo.vertexAttributeDescriptionCount = vertexDescriptor->attributeDescriptionCount();
        vertexInputInfo.pVertexAttributeDescriptions = vertexDescriptor->attributeDescriptionsData();
    }

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);
    shaderStages[0] = vertexShaderModule->stageInfo();
    shaderStages[1] = fragmentShaderModule->stageInfo();

    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = shaderStages.size();
    graphicsPipelineCreateInfo.pStages = shaderStages.data();
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
    graphicsPipelineCreateInfo.pViewportState = &configInfo.viewportInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &configInfo.rasterizationInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &configInfo.multisampleInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &configInfo.colorBlendInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &configInfo.depthStencilInfo;
    graphicsPipelineCreateInfo.pDynamicState = &configInfo.dynamicStateInfo;

    graphicsPipelineCreateInfo.layout = _pipelineLayout->pipelineLayout();
    graphicsPipelineCreateInfo.renderPass = renderPass->renderPass();
    graphicsPipelineCreateInfo.subpass = createInfo.subpassIndex;

    graphicsPipelineCreateInfo.basePipelineIndex = -1;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(g_vulkan_device->device(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &_graphicsPipeline));
}

void GraphicsPipeline::recompile() {
    vkDestroyPipeline(g_vulkan_device->device(), _graphicsPipeline, nullptr);
    throw std::runtime_error("Not implemented yet");
    //compile();
}

} //namespace vulkan

} //namespace lv
