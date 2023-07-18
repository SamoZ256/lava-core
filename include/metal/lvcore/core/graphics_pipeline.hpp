#ifndef LV_METAL_GRAPHICS_PIPELINE_H
#define LV_METAL_GRAPHICS_PIPELINE_H

#include "shader_module.hpp"
#include "render_pass.hpp"
#include "vertex_descriptor.hpp"
#include "pipeline_layout.hpp"

namespace lv {
    
struct Metal_GraphicsPipelineCreateInfo {
    Metal_ShaderModule* vertexShaderModule;
    //Metal_ShaderModule* tessellationControlShaderModule = nullptr;
    //Metal_ShaderModule* tessellationEvaluationShaderModule = nullptr;
    Metal_ShaderModule* fragmentShaderModule;
    Metal_PipelineLayout* pipelineLayout;
    Metal_RenderPass* renderPass;
    uint8_t subpassIndex = 0;
    Metal_VertexDescriptor* vertexDescriptor = nullptr;
    std::vector<Metal_ColorBlendAttachment> colorBlendAttachments;

    LvCullModeFlags cullMode = LV_CULL_MODE_NONE;
    LvBool depthTestEnable = LV_FALSE;
    LvBool depthWriteEnable = LV_TRUE;
    LvCompareOp depthOp = LV_COMPARE_OP_LESS;
    LvWindingOrder windingOrder = LV_WINDING_ORDER_CLOCKWISE;
    LvTessellationSpacing tessellationSpacing = LV_TESSELLATION_SPACING_EQUAL;
    uint32_t maxTessellationFactor = 16;
};

class Metal_GraphicsPipeline {
private:
    id /*MTLRenderPipelineState*/ _graphicsPipeline;

    id /*MTLDepthStencilState*/ _depthStencilState;

    _LV_MTL_RENDER_PIPELINE_DESCRIPTOR_T descriptor;

    Metal_ShaderModule* _vertexShaderModule;
    Metal_ShaderModule* _fragmentShaderModule;
    Metal_PipelineLayout* _pipelineLayout;

    LvCullModeFlags _cullMode;

public:
    Metal_GraphicsPipeline(Metal_GraphicsPipelineCreateInfo createInfo);

    ~Metal_GraphicsPipeline();

    void compile(Metal_GraphicsPipelineCreateInfo& createInfo);

    void recompile();

    //Getters
    inline id /*MTLRenderPipelineState*/ graphicsPipeline() { return _graphicsPipeline; }

    inline id /*MTLDepthStencilState*/ depthStencilState() { return _depthStencilState; }

    inline Metal_ShaderModule* vertexShaderModule() { return _vertexShaderModule; }

    inline Metal_ShaderModule* fragmentShaderModule() { return _fragmentShaderModule; }

    inline Metal_PipelineLayout* pipelineLayout() { return _pipelineLayout; }

    inline LvCullModeFlags cullMode() { return _cullMode; }
};

} //namespace lv

#endif
