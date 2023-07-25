#ifndef LV_VULKAN_COMPUTE_PIPELINE_H
#define LV_VULKAN_COMPUTE_PIPELINE_H

#include "lvcore/internal/compute_pipeline.hpp"

#include "shader_module.hpp"
#include "descriptor_set.hpp"

namespace lv {

namespace vulkan {

class ComputePipeline : public internal::ComputePipeline {
private:
    VkPipeline _computePipeline;

    ShaderModule* _computeShaderModule;
    PipelineLayout* _pipelineLayout;

public:
    ComputePipeline(internal::ComputePipelineCreateInfo createInfo);

    ~ComputePipeline() override;

    void compile();

    void recompile() override;

    //Getters
    inline VkPipeline computePipeline() { return _computePipeline; }

    inline ShaderModule* computeShaderModule() { return _computeShaderModule; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace vulkan

} //namespace lv

#endif
