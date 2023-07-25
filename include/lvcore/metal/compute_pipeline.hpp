#ifndef LV_METAL_COMPUTE_PIPELINE_H
#define LV_METAL_COMPUTE_PIPELINE_H

#include "lvcore/internal/compute_pipeline.hpp"

#include "shader_module.hpp"
#include "pipeline_layout.hpp"

namespace lv {

namespace metal {

class ComputePipeline : public internal::ComputePipeline {
private:
    id /*MTLComputePipelineState*/ _computePipeline = nullptr;

    ShaderModule* _computeShaderModule;
    PipelineLayout* _pipelineLayout;
    
    Bool threadGroupSizeIsMultipleOfThreadExecutionWidth;

public:
    ComputePipeline(internal::ComputePipelineCreateInfo createInfo);

    ~ComputePipeline() override;

    void compile();

    void recompile() override;

    //Getters
    inline id /*MTLComputePipelineState*/ computePipeline() { return _computePipeline; }

    inline ShaderModule* computeShaderModule() { return _computeShaderModule; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace metal

} //namespace lv

#endif
