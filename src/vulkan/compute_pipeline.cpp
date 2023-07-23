#include "vulkan/lvcore/core/compute_pipeline.hpp"

#include "vulkan/lvcore/core/device.hpp"

namespace lv {

Vulkan_ComputePipeline::Vulkan_ComputePipeline(Vulkan_ComputePipelineCreateInfo createInfo) {
    _computeShaderModule = createInfo.computeShaderModule;
    _pipelineLayout = createInfo.pipelineLayout;

    compile();
}

Vulkan_ComputePipeline::~Vulkan_ComputePipeline() {
    vkDestroyPipeline(g_vulkan_device->device(), _computePipeline, nullptr);
}

void Vulkan_ComputePipeline::compile() {
    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = _computeShaderModule->stageInfo();
    pipelineInfo.layout = _pipelineLayout->pipelineLayout();
    VK_CHECK_RESULT(vkCreateComputePipelines(g_vulkan_device->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_computePipeline));
}

void Vulkan_ComputePipeline::recompile() {
    vkDestroyPipeline(g_vulkan_device->device(), _computePipeline, nullptr);
    compile();
}

} //namespace lv
