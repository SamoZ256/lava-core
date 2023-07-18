#include "metal/lvcore/core/compute_pipeline.hpp"

#include "metal/lvcore/core/swap_chain.hpp"
#include "metal/lvcore/core/device.hpp"

namespace lv {

Metal_ComputePipeline::Metal_ComputePipeline(Metal_ComputePipelineCreateInfo createInfo) {
    _computeShaderModule = createInfo.computeShaderModule;
    _pipelineLayout = createInfo.pipelineLayout;
    threadGroupSizeIsMultipleOfThreadExecutionWidth = createInfo.threadGroupSizeIsMultipleOfThreadExecutionWidth;

    compile();
}

Metal_ComputePipeline::~Metal_ComputePipeline() {
    [_computePipeline release];
}

void Metal_ComputePipeline::compile() {
    MTLComputePipelineDescriptor* computePipelineDesc = [[MTLComputePipelineDescriptor alloc] init];
    computePipelineDesc.threadGroupSizeIsMultipleOfThreadExecutionWidth = threadGroupSizeIsMultipleOfThreadExecutionWidth;
    computePipelineDesc.computeFunction = _computeShaderModule->function();
    
    NSError* error;
    _computePipeline = [g_metal_device->device() newComputePipelineStateWithDescriptor:computePipelineDesc
                                                                            options:0
                                                                         reflection:nullptr
                                                                              error:&error];
    if (!_computePipeline) {
        throw std::runtime_error([[error localizedDescription] UTF8String]);
    }

    [computePipelineDesc release];
}

void Metal_ComputePipeline::recompile() {
    [_computePipeline release];
    compile();
}

} //namespace lv
