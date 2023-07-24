#ifndef LV_INTERNAL_GRAPHICS_PIPELINE_H
#define LV_INTERNAL_GRAPHICS_PIPELINE_H

#include "pipeline_layout.hpp"
#include "render_pass.hpp"
#include "shader_module.hpp"
#include "vertex_descriptor.hpp"

namespace lv {

namespace internal {

struct GraphicsPipelineCreateInfo {
    ShaderModule* vertexShaderModule;
    //ShaderModule* tessellationControlShaderModule = nullptr;
    //ShaderModule* tessellationEvaluationShaderModule = nullptr;
    ShaderModule* fragmentShaderModule;
    PipelineLayout* pipelineLayout;
    RenderPass* renderPass;
    uint8_t subpassIndex = 0;
    VertexDescriptor* vertexDescriptor = nullptr;
    std::vector<ColorBlendAttachment> colorBlendAttachments;

    CullMode cullMode = CullMode::None;
    Bool depthTestEnable = False;
    Bool depthWriteEnable = True;
    CompareOperation depthOp = CompareOperation::Less;
    FrontFace frontFace = FrontFace::Clockwise; //TODO: use this without tessellation as well
    TessellationSpacing tessellationSpacing = TessellationSpacing::Equal;
    uint32_t maxTessellationFactor = 16;
};

class GraphicsPipeline {
public:
    virtual ~GraphicsPipeline() {}

    virtual void recompile() = 0;
};

} //namespace internal

} //namespace lv

#endif
