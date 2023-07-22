#ifndef LV_VULKAN_GRAPHICS_PIPELINE_H
#define LV_VULKAN_GRAPHICS_PIPELINE_H

#include <cassert>

#include "render_pass.hpp"
#include "descriptor_set.hpp"
#include "vertex_descriptor.hpp"
#include "shader_module.hpp"

namespace lv {

struct Vulkan_PipelineConfigInfo {
    VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    std::vector<VkDynamicState> dynamicStates;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

struct Vulkan_GraphicsPipelineCreateInfo {
    Vulkan_ShaderModule* vertexShaderModule = nullptr;
    Vulkan_ShaderModule* fragmentShaderModule = nullptr;
    Vulkan_PipelineLayout* pipelineLayout = nullptr;
    Vulkan_RenderPass* renderPass = nullptr;
    uint8_t subpassIndex = 0;
    Vulkan_VertexDescriptor* vertexDescriptor = nullptr;
    std::vector<Vulkan_ColorBlendAttachment> colorBlendAttachments;

    CullMode cullMode = CullMode::None;
    Bool depthTestEnable = False;
    Bool depthWriteEnable = True;
    CompareOperation depthOp = CompareOperation::Less;
};

class Vulkan_GraphicsPipeline {
private:
    VkPipeline _graphicsPipeline;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

    Vulkan_ShaderModule* vertexShaderModule;
    Vulkan_ShaderModule* fragmentShaderModule;
    Vulkan_PipelineLayout* _pipelineLayout;

public:
    Vulkan_GraphicsPipeline(Vulkan_GraphicsPipelineCreateInfo createInfo);

    ~Vulkan_GraphicsPipeline();

    void compile(Vulkan_GraphicsPipelineCreateInfo& createInfo);

    void recompile();

    //Getters
    inline VkPipeline graphicsPipeline() { return _graphicsPipeline; }

    inline Vulkan_PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace lv

#endif
