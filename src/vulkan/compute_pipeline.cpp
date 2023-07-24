#include "vulkan/lvcore/compute_pipeline.hpp"

#include "vulkan/lvcore/device.hpp"

namespace lv {

namespace vulkan {

ComputePipeline::ComputePipeline(ComputePipelineCreateInfo createInfo) {
    _computeShaderModule = createInfo.computeShaderModule;
    _pipelineLayout = createInfo.pipelineLayout;

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
