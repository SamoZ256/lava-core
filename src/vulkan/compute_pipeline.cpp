#include "lvcore/vulkan/compute_pipeline.hpp"

#include "lvcore/vulkan/device.hpp"

namespace lv {

namespace vulkan {

ComputePipeline::ComputePipeline(internal::ComputePipelineCreateInfo createInfo) {
    _computeShaderModule = static_cast<ShaderModule*>(createInfo.computeShaderModule);
    _pipelineLayout = static_cast<PipelineLayout*>(createInfo.pipelineLayout);

    compile();
}

ComputePipeline::~ComputePipeline() {
    vkDestroyPipeline(g_vulkan_device->device(), _computePipeline, nullptr);
}

void ComputePipeline::compile() {
    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = _computeShaderModule->stageInfo();
    pipelineInfo.layout = _pipelineLayout->pipelineLayout();
    VK_CHECK_RESULT(vkCreateComputePipelines(g_vulkan_device->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_computePipeline));
}

void ComputePipeline::recompile() {
    vkDestroyPipeline(g_vulkan_device->device(), _computePipeline, nullptr);
    compile();
}

} //namespace vulkan

} //namespace lv
