#include "lvcore/metal/command_buffer.hpp"

#include "lvcore/metal/core.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "lvcore/metal/device.hpp"
#include "lvcore/metal/swap_chain.hpp"

namespace lv {

namespace metal {

#define _LV_CHECK_IF_ENCODING \
if (isEncoding) { \
    [encoder endEncoding]; \
    [encoder release]; \
    isEncoding = false; \
}

CommandBuffer::CommandBuffer(internal::CommandBufferCreateInfo createInfo) {
    frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);

    commandBuffers.resize(frameCount);
}

void CommandBuffer::beginRecording(CommandBufferUsageFlags usage) {
    commandBuffers[g_metal_swapChain->crntFrame() % frameCount] = [g_metal_device->commandQueue() commandBuffer];
}

void CommandBuffer::endRecording() {
    _LV_CHECK_IF_ENCODING;
}

void CommandBuffer::submit(internal::Semaphore* waitSemaphore, internal::Semaphore* signalSemaphore) {
    CAST_FROM_INTERNAL(waitSemaphore, Semaphore);
    CAST_FROM_INTERNAL(signalSemaphore, Semaphore);

    uint8_t index = g_metal_swapChain->crntFrame() % frameCount;

    if (waitSemaphore_ != nullptr)
        dispatch_semaphore_wait((dispatch_semaphore_t)waitSemaphore_->semaphore(), DISPATCH_TIME_FOREVER);
    
    if (signalSemaphore_ != nullptr) {
        [commandBuffers[index] addCompletedHandler:^(id<MTLCommandBuffer> cmd) {
            dispatch_semaphore_signal((dispatch_semaphore_t)signalSemaphore_->semaphore());
        }];
    }

    [commandBuffers[index] commit];
    [commandBuffers[index] release];
}

void CommandBuffer::beginRenderCommands(internal::Framebuffer* framebuffer) {
    CAST_FROM_INTERNAL(framebuffer, Framebuffer);

    _LV_CHECK_IF_ENCODING;
    MTLRenderPassDescriptor* renderPassDescriptor = (MTLRenderPassDescriptor*)framebuffer_->renderPass(g_metal_swapChain->crntFrame() % framebuffer_->frameCount());
    encoder = [commandBuffers[g_metal_swapChain->crntFrame() % frameCount] renderCommandEncoderWithDescriptor:renderPassDescriptor];
    activeRenderPass = renderPassDescriptor;
    isEncoding = true;
}

void CommandBuffer::beginComputeCommands() {
    _LV_CHECK_IF_ENCODING;
    encoder = [commandBuffers[g_metal_swapChain->crntFrame() % frameCount] computeCommandEncoder];
    isEncoding = true;
}

void CommandBuffer::beginBlitCommands() {
    _LV_CHECK_IF_ENCODING;
    encoder = [commandBuffers[g_metal_swapChain->crntFrame() % frameCount] blitCommandEncoder];
    isEncoding = true;
}

void CommandBuffer::endCommands() {
    _LV_CHECK_IF_ENCODING;
}

void CommandBuffer::waitUntilCompleted() {
    //[_getActiveCommandBuffer() waitUntilCompleted];
}

id CommandBuffer::_getActiveCommandBuffer() {
    return commandBuffers[g_metal_swapChain->crntFrame() % frameCount];
}

//Commands

//Render
void CommandBuffer::cmdBindVertexBuffer(internal::Buffer* buffer) {
    CAST_FROM_INTERNAL(buffer, Buffer);

    [encoder setVertexBuffer:buffer_->buffer(g_metal_swapChain->crntFrame() % buffer_->frameCount())
                      offset:0
                     atIndex:LV_METAL_VERTEX_BUFFER_BINDING_INDEX];
}

void CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount) {
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                vertexStart:0
                vertexCount:vertexCount
              instanceCount:instanceCount];
}

void CommandBuffer::cmdDrawIndexed(internal::Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount) {
    CAST_FROM_INTERNAL(indexBuffer, Buffer);

    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                        indexCount:indexCount
                         indexType:(MTLIndexType)indexType
                       indexBuffer:indexBuffer_->buffer(g_metal_swapChain->crntFrame() % indexBuffer_->frameCount())
                 indexBufferOffset:0
                     instanceCount:instanceCount];
}

void CommandBuffer::cmdDrawPatches(uint32_t controlPointCount, uint32_t patchCount, uint32_t instanceCount) {
    [encoder drawPatches:controlPointCount patchStart:0 patchCount:patchCount patchIndexBuffer:NULL patchIndexBufferOffset:0 instanceCount:instanceCount baseInstance:0];
}

#define GET_BINDING_BY_SHADER_STAGE_INDEX(shaderStageIndex, bindingsName, bindingIndicesName) \
activeShaderModules[shaderStageIndex]->bindingsName[descriptorSet_->layoutIndex()][descriptorSet_->bindingIndicesName[i]]

void CommandBuffer::cmdBindDescriptorSet(internal::DescriptorSet* descriptorSet) {
    CAST_FROM_INTERNAL(descriptorSet, DescriptorSet);

    for (uint32_t i = 0; i < descriptorSet_->buffers.size(); i++) {
        id<MTLBuffer> buffer = descriptorSet_->buffers[i][g_metal_swapChain->crntFrame() % descriptorSet_->buffers[i].size()];
        ShaderStageFlags shaderStage = descriptorSet_->pipelineLayout()->descriptorSetLayouts[descriptorSet_->layoutIndex()].bindings[descriptorSet_->bufferBindingIndices[i]].shaderStage;

        if (shaderStage & ShaderStageFlags::Vertex) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_VERTEX_INDEX, bufferBindings, bufferBindingIndices);
            [encoder setVertexBuffer:buffer
                                                             offset:0
                                                            atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Fragment) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_FRAGMENT_INDEX, bufferBindings, bufferBindingIndices);
            [encoder setFragmentBuffer:buffer
                                                               offset:0
                                                              atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Compute) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_COMPUTE_INDEX, bufferBindings, bufferBindingIndices);
            [encoder setBuffer:buffer
                                                        offset:0
                                                       atIndex:binding];
        }
    }

    for (uint32_t i = 0; i < descriptorSet_->textures.size(); i++) {
        id<MTLTexture> texture = descriptorSet_->textures[i][g_metal_swapChain->crntFrame() % descriptorSet_->textures[i].size()];
        ShaderStageFlags shaderStage = descriptorSet_->pipelineLayout()->descriptorSetLayouts[descriptorSet_->layoutIndex()].bindings[descriptorSet_->textureBindingIndices[i]].shaderStage;

        if (shaderStage & ShaderStageFlags::Vertex) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_VERTEX_INDEX, textureBindings, textureBindingIndices);
            [encoder setVertexTexture:texture
                                                             atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Fragment) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_FRAGMENT_INDEX, textureBindings, textureBindingIndices);
            [encoder setFragmentTexture:texture
                                                               atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Compute) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_COMPUTE_INDEX, textureBindings, textureBindingIndices);
            [encoder setTexture:texture
                                                        atIndex:binding];
        }
    }

    for (uint32_t i = 0; i < descriptorSet_->samplers.size(); i++) {
        ShaderStageFlags shaderStage = descriptorSet_->pipelineLayout()->descriptorSetLayouts[descriptorSet_->layoutIndex()].bindings[descriptorSet_->samplerBindingIndices[i]].shaderStage;

        if (shaderStage & ShaderStageFlags::Vertex) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_VERTEX_INDEX, samplerBindings, samplerBindingIndices);
            [encoder setVertexSamplerState:descriptorSet_->samplers[i]
                                                                  atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Fragment) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_FRAGMENT_INDEX, samplerBindings, samplerBindingIndices);
            [encoder setFragmentSamplerState:descriptorSet_->samplers[i]
                                                                    atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Compute) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_COMPUTE_INDEX, samplerBindings, samplerBindingIndices);
            [encoder setSamplerState:descriptorSet_->samplers[i]
                                                             atIndex:binding];
        }
    }
}

void CommandBuffer::cmdBindGraphicsPipeline(internal::GraphicsPipeline* graphicsPipeline) {
    CAST_FROM_INTERNAL(graphicsPipeline, GraphicsPipeline);

    [encoder setRenderPipelineState:graphicsPipeline_->graphicsPipeline()];
    [encoder setDepthStencilState:graphicsPipeline_->depthStencilState()];
    MTLCullMode mtlCullMode;
    GET_MTL_CULL_MODE(graphicsPipeline_->cullMode(), mtlCullMode);
    [encoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [encoder setCullMode:mtlCullMode];
    activePipelineLayout = graphicsPipeline_->pipelineLayout();
    activeShaderModules[LV_SHADER_STAGE_VERTEX_INDEX] = graphicsPipeline_->vertexShaderModule();
    activeShaderModules[LV_SHADER_STAGE_FRAGMENT_INDEX] = graphicsPipeline_->fragmentShaderModule();
}

void CommandBuffer::cmdPushConstants(void* data, uint16_t index) {
    size_t size = activePipelineLayout->pushConstantRanges[index].size;
    ShaderStageFlags shaderStage = activePipelineLayout->pushConstantRanges[index].stageFlags;

    if (shaderStage & ShaderStageFlags::Vertex) {
        uint32_t bufferIndex = activeShaderModules[LV_SHADER_STAGE_VERTEX_INDEX]->pushConstantBinding;
        [encoder setVertexBytes:data
                                                        length:roundToMultipleOf16(size)
                                                       atIndex:bufferIndex];
    }
    if (shaderStage & ShaderStageFlags::Fragment) {
        uint32_t bufferIndex = activeShaderModules[LV_SHADER_STAGE_FRAGMENT_INDEX]->pushConstantBinding;
        [encoder setFragmentBytes:data
                                                          length:roundToMultipleOf16(size)
                                                         atIndex:bufferIndex];
    }
    if (shaderStage & ShaderStageFlags::Compute) {
        uint32_t bufferIndex = activeShaderModules[LV_SHADER_STAGE_COMPUTE_INDEX]->pushConstantBinding;
        [encoder setBytes:data
                                                          length:roundToMultipleOf16(size)
                                                         atIndex:bufferIndex];
    }
}

void CommandBuffer::cmdBindViewport(internal::Viewport* viewport) {
    //TODO: fix this
    //[encoder setViewport:viewport];
    //[encoder setScissorRect:scissor];
}

void CommandBuffer::cmdBindTessellationFactorBuffer(internal::Buffer* buffer) {
    CAST_FROM_INTERNAL(buffer, Buffer);

    [encoder setTessellationFactorBuffer:buffer_->buffer(g_metal_swapChain->crntFrame() % buffer_->frameCount()) offset:0 instanceStride:0];
}

void CommandBuffer::cmdBindTessellationFactorBufferForWriting(internal::Buffer* buffer) {
    CAST_FROM_INTERNAL(buffer, Buffer);

    [encoder setBuffer:buffer_->buffer(g_metal_swapChain->crntFrame() % buffer_->frameCount())
                offset:0
               atIndex:26]; //TODO: check whether it's always 26
}

void CommandBuffer::cmdEnableWireframeMode() {
    [encoder setTriangleFillMode:MTLTriangleFillModeLines];
}

//Compute
void CommandBuffer::cmdBindComputePipeline(internal::ComputePipeline* computePipeline) {
    CAST_FROM_INTERNAL(computePipeline, ComputePipeline);

    [encoder setComputePipelineState:computePipeline_->computePipeline()];
    activePipelineLayout = computePipeline_->pipelineLayout();
    activeShaderModules[LV_SHADER_STAGE_COMPUTE_INDEX] = computePipeline_->computeShaderModule();
}

void CommandBuffer::cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ, uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ) {
    [encoder dispatchThreadgroups:MTLSizeMake(threadgroupsX, threadgroupsY, threadgroupsZ)
            threadsPerThreadgroup:MTLSizeMake(threadsPerGroupX, threadsPerGroupY, threadsPerGroupZ)];
}

void CommandBuffer::cmdDispatchTessellationControl(uint32_t patchCount) {
    [encoder dispatchThreadgroups:MTLSizeMake(patchCount, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(1, 1, 1)]; //TODO: add option to dispatch for every control point
}

//Blit
void CommandBuffer::cmdStagingCopyDataToBuffer(internal::Buffer* buffer, void* data, size_t aSize) {
    CAST_FROM_INTERNAL(buffer, Buffer);

    if (aSize == 0)
        aSize = buffer_->size();
    uint8_t index = g_metal_swapChain->crntFrame() % buffer_->frameCount();
    
    id<MTLBuffer> stagingBuffer = [g_metal_device->device() newBufferWithLength:aSize
                                                                        options:MTLResourceStorageModeShared];
    memcpy([stagingBuffer contents], data, aSize);

    Buffer::copyBufferToBuffer(encoder, stagingBuffer, buffer_->buffer(index), aSize);

    [stagingBuffer release];
}

void CommandBuffer::cmdStagingCopyDataToImage(internal::Image* image, void* data, uint8_t bytesPerPixel) {
    CAST_FROM_INTERNAL(image, Image);

    size_t size = image_->width() * image_->height() * bytesPerPixel;
    id<MTLBuffer> stagingBuffer = [g_metal_device->device() newBufferWithLength:size
                                                                      options:MTLResourceStorageModeShared];
    memcpy([stagingBuffer contents], data, size);

    for (uint8_t i = 0; i < image_->frameCount(); i++)
        Buffer::copyBufferToImage(encoder, stagingBuffer, image_->image(i), image_->width(), image_->height(), bytesPerPixel);

    [stagingBuffer release];
}

void CommandBuffer::cmdGenerateMipmapsForImage(internal::Image* image, uint8_t aFrameCount) {
    CAST_FROM_INTERNAL(image, Image);

    if (aFrameCount == 0) aFrameCount = image_->frameCount();

    for (uint8_t i = 0; i < aFrameCount; i++)
        [encoder generateMipmapsForTexture:image_->image(g_metal_swapChain->crntFrame() + i)];
}

void CommandBuffer::cmdCopyToImageFromImage(internal::Image* source, internal::Image* destination) {
    CAST_FROM_INTERNAL(source, Image);
    CAST_FROM_INTERNAL(destination, Image);

    [encoder copyFromTexture:source_->image(g_metal_swapChain->crntFrame() % source_->frameCount())
                              toTexture:destination_->image(g_metal_swapChain->crntFrame() % destination_->frameCount())];
}

void CommandBuffer::cmdBlitToImageFromImage(internal::Image* source, internal::Image* destination) {
    CAST_FROM_INTERNAL(source, Image);
    CAST_FROM_INTERNAL(destination, Image);

    if (!Image::blitComputePipelineState) {
        std::string libSource = readFile("/Users/samuliak/Documents/lava_core/src/metal/blit.metallib"); //TODO: load this file independently of path

        dispatch_data_t sourceData = dispatch_data_create((void*)libSource.c_str(), libSource.size() * sizeof(char), nullptr, nullptr);

        //Library
        NSError* error;
        Image::blitComputeLibrary = [g_metal_device->device() newLibraryWithData:sourceData
                                                    error:&error];
        if (!Image::blitComputeLibrary) {
            throw std::runtime_error([[error localizedDescription] UTF8String]);
        }

        //Function
        Image::blitComputeFunction = [Image::blitComputeLibrary newFunctionWithName:@"main0"];

        //Pipeline state
        MTLComputePipelineDescriptor* computePipelineDesc = [[MTLComputePipelineDescriptor alloc] init];
        computePipelineDesc.threadGroupSizeIsMultipleOfThreadExecutionWidth = true;
        computePipelineDesc.computeFunction = Image::blitComputeFunction;
        
        Image::blitComputePipelineState = [g_metal_device->device() newComputePipelineStateWithDescriptor:computePipelineDesc
                                                                                         options:0
                                                                                      reflection:nullptr
                                                                                           error:&error];
        if (!Image::blitComputePipelineState) {
            throw std::runtime_error([[error localizedDescription] UTF8String]);
        }

        [computePipelineDesc release];
    }

    [encoder setComputePipelineState:Image::blitComputePipelineState];

    [encoder setTexture:source_->image(g_metal_swapChain->crntFrame() % source_->frameCount())
                atIndex:0];
    [encoder setTexture:destination_->image(g_metal_swapChain->crntFrame() % destination_->frameCount())
                atIndex:1];

    uint8_t threadGroupSize = 32;
    [encoder dispatchThreadgroups:MTLSizeMake(threadGroupSize, threadGroupSize, 1)
            threadsPerThreadgroup:MTLSizeMake((destination_->width() + threadGroupSize - 1) / threadGroupSize, (destination_->height() + threadGroupSize - 1) / threadGroupSize, 1)];
}

//Other
void CommandBuffer::cmdPresent() {
    [_getActiveCommandBuffer() presentDrawable:g_metal_swapChain->drawable()];
}

} //namespace metal

} //namespace lv
