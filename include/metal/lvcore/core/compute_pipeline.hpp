#ifndef LV_METAL_COMPUTE_PIPELINE_H
#define LV_METAL_COMPUTE_PIPELINE_H

#include "shader_module.hpp"
#include "pipeline_layout.hpp"

namespace lv {

struct Metal_ComputePipelineCreateInfo {
    Metal_ShaderModule* computeShaderModule;
    Metal_PipelineLayout* pipelineLayout;
    bool threadGroupSizeIsMultipleOfThreadExecutionWidth = true;
};

class Metal_ComputePipeline {
private:
    id /*MTLComputePipelineState*/ _computePipeline = nullptr;

    Metal_ShaderModule* _computeShaderModule;
    Metal_PipelineLayout* _pipelineLayout;
    
    bool threadGroupSizeIsMultipleOfThreadExecutionWidth;

public:
    Metal_ComputePipeline(Metal_ComputePipelineCreateInfo createInfo);

    ~Metal_ComputePipeline();

    void compile();

    void recompile();

    //Getters
    inline id /*MTLComputePipelineState*/ computePipeline() { return _computePipeline; }

    inline Metal_ShaderModule* computeShaderModule() { return _computeShaderModule; }

    inline Metal_PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace lv

#endif
