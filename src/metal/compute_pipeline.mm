#include "metal/lvcore/compute_pipeline.hpp"

#include "metal/lvcore/swap_chain.hpp"
#include "metal/lvcore/device.hpp"

namespace lv {

namespace metal {

ComputePipeline::ComputePipeline(internal::ComputePipelineCreateInfo createInfo) {
    _computeShaderModule = (ShaderModule*)createInfo.computeShaderModule;
    _pipelineLayout = (PipelineLayout*)createInfo.pipelineLayout;
    threadGroupSizeIsMultipleOfThreadExecutionWidth = createInfo.threadGroupSizeIsMultipleOfThreadExecutionWidth;

    compile();
}

ComputePipeline::~ComputePipeline() {
    [_computePipeline release];
}

void ComputePipeline::compile() {
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

void ComputePipeline::recompile() {
    [_computePipeline release];
    compile();
}

} //namespace metal

} //namespace lv
