#ifndef LV_METAL_GRAPHICS_PIPELINE_H
#define LV_METAL_GRAPHICS_PIPELINE_H

#include "lvcore/internal/graphics_pipeline.hpp"

#include "shader_module.hpp"
#include "render_pass.hpp"
#include "vertex_descriptor.hpp"
#include "pipeline_layout.hpp"

namespace lv {

namespace metal {

class GraphicsPipeline : public internal::GraphicsPipeline {
private:
    id /*MTLRenderPipelineState*/ _graphicsPipeline;

    id /*MTLDepthStencilState*/ _depthStencilState;

    _LV_MTL_RENDER_PIPELINE_DESCRIPTOR_T descriptor;

    ShaderModule* _vertexShaderModule;
    ShaderModule* _fragmentShaderModule;
    PipelineLayout* _pipelineLayout;

    CullMode _cullMode;

public:
    GraphicsPipeline(internal::GraphicsPipelineCreateInfo createInfo);

    ~GraphicsPipeline() override;

    void compile(internal::GraphicsPipelineCreateInfo& createInfo);

    void recompile() override;

    //Getters
    inline id /*MTLRenderPipelineState*/ graphicsPipeline() { return _graphicsPipeline; }

    inline id /*MTLDepthStencilState*/ depthStencilState() { return _depthStencilState; }

    inline ShaderModule* vertexShaderModule() { return _vertexShaderModule; }

    inline ShaderModule* fragmentShaderModule() { return _fragmentShaderModule; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }

    inline CullMode cullMode() { return _cullMode; }
};

} //namespace metal

} //namespace lv

#endif
