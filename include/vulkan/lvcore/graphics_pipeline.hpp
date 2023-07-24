#ifndef LV_VULKAN_GRAPHICS_PIPELINE_H
#define LV_VULKAN_GRAPHICS_PIPELINE_H

#include <cassert>

#include "render_pass.hpp"
#include "descriptor_set.hpp"
#include "vertex_descriptor.hpp"
#include "shader_module.hpp"

namespace lv {

namespace vulkan {

struct PipelineConfigInfo {
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

struct GraphicsPipelineCreateInfo {
    ShaderModule* vertexShaderModule = nullptr;
    ShaderModule* fragmentShaderModule = nullptr;
    PipelineLayout* pipelineLayout = nullptr;
    RenderPass* renderPass = nullptr;
    uint8_t subpassIndex = 0;
    VertexDescriptor* vertexDescriptor = nullptr;
    std::vector<ColorBlendAttachment> colorBlendAttachments;

    CullMode cullMode = CullMode::None;
    Bool depthTestEnable = False;
    Bool depthWriteEnable = True;
    CompareOperation depthOp = CompareOperation::Less;
};

class GraphicsPipeline {
private:
    VkPipeline _graphicsPipeline;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

    ShaderModule* vertexShaderModule;
    ShaderModule* fragmentShaderModule;
    PipelineLayout* _pipelineLayout;

public:
    GraphicsPipeline(GraphicsPipelineCreateInfo createInfo);

    ~GraphicsPipeline();

    void compile(GraphicsPipelineCreateInfo& createInfo);

    void recompile();

    //Getters
    inline VkPipeline graphicsPipeline() { return _graphicsPipeline; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace vulkan

} //namespace lv

#endif
