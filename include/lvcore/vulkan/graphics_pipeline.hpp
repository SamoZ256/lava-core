#ifndef LV_VULKAN_GRAPHICS_PIPELINE_H
#define LV_VULKAN_GRAPHICS_PIPELINE_H

#include "lvcore/internal/graphics_pipeline.hpp"

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

class GraphicsPipeline : public internal::GraphicsPipeline {
private:
    VkPipeline _graphicsPipeline;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

    ShaderModule* vertexShaderModule;
    ShaderModule* fragmentShaderModule;
    PipelineLayout* _pipelineLayout;

public:
    GraphicsPipeline(internal::GraphicsPipelineCreateInfo createInfo);

    ~GraphicsPipeline() override;

    void compile(internal::GraphicsPipelineCreateInfo& createInfo);

    void recompile() override;

    //Getters
    inline VkPipeline graphicsPipeline() { return _graphicsPipeline; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace vulkan

} //namespace lv

#endif
