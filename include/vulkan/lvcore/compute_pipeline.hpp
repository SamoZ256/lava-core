#ifndef LV_VULKAN_COMPUTE_PIPELINE_H
#define LV_VULKAN_COMPUTE_PIPELINE_H

#include "shader_module.hpp"
#include "descriptor_set.hpp"

namespace lv {

namespace vulkan {

struct ComputePipelineCreateInfo {
    ShaderModule* computeShaderModule;
    PipelineLayout* pipelineLayout;
};

class ComputePipeline {
private:
    VkPipeline _computePipeline;

    ShaderModule* _computeShaderModule;
    PipelineLayout* _pipelineLayout;

public:
    ComputePipeline(ComputePipelineCreateInfo createInfo);

    ~ComputePipeline();

    void compile();

    void recompile();

    //Getters
    inline VkPipeline computePipeline() { return _computePipeline; }

    inline ShaderModule* computeShaderModule() { return _computeShaderModule; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace vulkan

} //namespace lv

#endif
