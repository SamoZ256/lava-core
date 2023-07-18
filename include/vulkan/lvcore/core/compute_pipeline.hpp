#ifndef LV_VULKAN_COMPUTE_PIPELINE_H
#define LV_VULKAN_COMPUTE_PIPELINE_H

#include "shader_module.hpp"
#include "descriptor_set.hpp"

namespace lv {

struct Vulkan_ComputePipelineCreateInfo {
    Vulkan_ShaderModule* computeShaderModule;
    Vulkan_PipelineLayout* pipelineLayout;
};

class Vulkan_ComputePipeline {
private:
    VkPipeline _computePipeline;

    Vulkan_ShaderModule* _computeShaderModule;
    Vulkan_PipelineLayout* _pipelineLayout;

public:
    Vulkan_ComputePipeline(Vulkan_ComputePipelineCreateInfo createInfo);

    ~Vulkan_ComputePipeline();

    void compile();

    void recompile();

    //Getters
    inline VkPipeline computePipeline() { return _computePipeline; }

    inline Vulkan_ShaderModule* computeShaderModule() { return _computeShaderModule; }

    inline Vulkan_PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace lv

#endif
