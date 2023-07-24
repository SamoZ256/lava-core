#ifndef LV_INTERNAL_COMPUTE_PIPELINE_H
#define LV_INTERNAL_COMPUTE_PIPELINE_H

#include "shader_module.hpp"
#include "pipeline_layout.hpp"

namespace lv {

namespace internal {

struct ComputePipelineCreateInfo {
    ShaderModule* computeShaderModule;
    PipelineLayout* pipelineLayout;
    Bool threadGroupSizeIsMultipleOfThreadExecutionWidth = True;
};

class ComputePipeline {
public:
    virtual ~ComputePipeline() {}

    virtual void recompile() = 0;
};

} //namespace internal

} //namespace lv

#endif
