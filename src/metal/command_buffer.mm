#include "metal/lvcore/core/command_buffer.hpp"

#include "metal/lvcore/core/core.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

#define _LV_CHECK_IF_ENCODING \
if (isEncoding) { \
    [encoder endEncoding]; \
    [encoder release]; \
    isEncoding = false; \
}

Metal_CommandBuffer::Metal_CommandBuffer(Metal_CommandBufferCreateInfo createInfo) {
    frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);

    commandBuffers.resize(frameCount);
}

void Metal_CommandBuffer::beginRecording(CommandBufferUsageFlags usage) {
    commandBuffers[g_metal_swapChain->crntFrame() % frameCount] = [g_metal_device->commandQueue() commandBuffer];
}

void Metal_CommandBuffer::endRecording() {
    _LV_CHECK_IF_ENCODING;
}

void Metal_CommandBuffer::submit(Metal_Semaphore* waitSemaphore, Metal_Semaphore* signalSemaphore) {
    uint8_t index = g_metal_swapChain->crntFrame() % frameCount;

    if (waitSemaphore != nullptr)
        dispatch_semaphore_wait((dispatch_semaphore_t)waitSemaphore->semaphore(), DISPATCH_TIME_FOREVER);
    
    if (signalSemaphore != nullptr) {
        [commandBuffers[index] addCompletedHandler:^(id<MTLCommandBuffer> cmd) {
            dispatch_semaphore_signal((dispatch_semaphore_t)signalSemaphore->semaphore());
        }];
    }

    [commandBuffers[index] commit];
    [commandBuffers[index] release];
}

void Metal_CommandBuffer::beginRenderCommands(Metal_Framebuffer* framebuffer) {
    _LV_CHECK_IF_ENCODING;
    MTLRenderPassDescriptor* renderPassDescriptor = (MTLRenderPassDescriptor*)framebuffer->renderPass(g_metal_swapChain->crntFrame() % framebuffer->frameCount());
    encoder = [commandBuffers[g_metal_swapChain->crntFrame() % frameCount] renderCommandEncoderWithDescriptor:renderPassDescriptor];
    activeRenderPass = renderPassDescriptor;
    isEncoding = true;
}

void Metal_CommandBuffer::beginComputeCommands() {
    _LV_CHECK_IF_ENCODING;
    encoder = [commandBuffers[g_metal_swapChain->crntFrame() % frameCount] computeCommandEncoder];
    isEncoding = true;
}

void Metal_CommandBuffer::beginBlitCommands() {
    _LV_CHECK_IF_ENCODING;
    encoder = [commandBuffers[g_metal_swapChain->crntFrame() % frameCount] blitCommandEncoder];
    isEncoding = true;
}

void Metal_CommandBuffer::endCommands() {
    _LV_CHECK_IF_ENCODING;
}

void Metal_CommandBuffer::waitUntilCompleted() {
    //[_getActiveCommandBuffer() waitUntilCompleted];
}

id Metal_CommandBuffer::_getActiveCommandBuffer() {
    return commandBuffers[g_metal_swapChain->crntFrame() % frameCount];
}

//Commands

//Render
void Metal_CommandBuffer::cmdBindVertexBuffer(Metal_Buffer* buffer) {
    [encoder setVertexBuffer:buffer->buffer(g_metal_swapChain->crntFrame() % buffer->frameCount())
                      offset:0
                     atIndex:LV_METAL_VERTEX_BUFFER_BINDING_INDEX];
}

void Metal_CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount) {
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                vertexStart:0
                vertexCount:vertexCount
              instanceCount:instanceCount];
}

void Metal_CommandBuffer::cmdDrawIndexed(Metal_Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount) {
    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                        indexCount:indexCount
                         indexType:(MTLIndexType)indexType
                       indexBuffer:indexBuffer->buffer(g_metal_swapChain->crntFrame() % indexBuffer->frameCount())
                 indexBufferOffset:0
                     instanceCount:instanceCount];
}

void Metal_CommandBuffer::cmdDrawPatches(uint32_t controlPointCount, uint32_t patchCount, uint32_t instanceCount) {
    [encoder drawPatches:controlPointCount patchStart:0 patchCount:patchCount patchIndexBuffer:NULL patchIndexBufferOffset:0 instanceCount:instanceCount baseInstance:0];
}

#define GET_BINDING_BY_SHADER_STAGE_INDEX(shaderStageIndex, bindingsName, bindingIndicesName) \
activeShaderModules[shaderStageIndex]->bindingsName[descriptorSet->layoutIndex()][descriptorSet->bindingIndicesName[i]]

void Metal_CommandBuffer::cmdBindDescriptorSet(Metal_DescriptorSet* descriptorSet) {
    for (uint32_t i = 0; i < descriptorSet->buffers.size(); i++) {
        id<MTLBuffer> buffer = descriptorSet->buffers[i][g_metal_swapChain->crntFrame() % descriptorSet->buffers[i].size()];
        ShaderStageFlags shaderStage = descriptorSet->pipelineLayout()->descriptorSetLayouts[descriptorSet->layoutIndex()].bindings[descriptorSet->bufferBindingIndices[i]].shaderStage;

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

    for (uint32_t i = 0; i < descriptorSet->textures.size(); i++) {
        id<MTLTexture> texture = descriptorSet->textures[i][g_metal_swapChain->crntFrame() % descriptorSet->textures[i].size()];
        ShaderStageFlags shaderStage = descriptorSet->pipelineLayout()->descriptorSetLayouts[descriptorSet->layoutIndex()].bindings[descriptorSet->textureBindingIndices[i]].shaderStage;

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

    for (uint32_t i = 0; i < descriptorSet->samplers.size(); i++) {
        ShaderStageFlags shaderStage = descriptorSet->pipelineLayout()->descriptorSetLayouts[descriptorSet->layoutIndex()].bindings[descriptorSet->samplerBindingIndices[i]].shaderStage;

        if (shaderStage & ShaderStageFlags::Vertex) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_VERTEX_INDEX, samplerBindings, samplerBindingIndices);
            [encoder setVertexSamplerState:descriptorSet->samplers[i]
                                                                  atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Fragment) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_FRAGMENT_INDEX, samplerBindings, samplerBindingIndices);
            [encoder setFragmentSamplerState:descriptorSet->samplers[i]
                                                                    atIndex:binding];
        }
        if (shaderStage & ShaderStageFlags::Compute) {
            uint32_t binding = GET_BINDING_BY_SHADER_STAGE_INDEX(LV_SHADER_STAGE_COMPUTE_INDEX, samplerBindings, samplerBindingIndices);
            [encoder setSamplerState:descriptorSet->samplers[i]
                                                             atIndex:binding];
        }
    }
}

void Metal_CommandBuffer::cmdBindGraphicsPipeline(Metal_GraphicsPipeline* graphicsPipeline) {
    [encoder setRenderPipelineState:graphicsPipeline->graphicsPipeline()];

    [encoder setDepthStencilState:graphicsPipeline->depthStencilState()];
    MTLCullMode mtlCullMode;
    GET_MTL_CULL_MODE(graphicsPipeline->cullMode(), mtlCullMode);
    [encoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [encoder setCullMode:mtlCullMode];
    activePipelineLayout = graphicsPipeline->pipelineLayout();
    activeShaderModules[LV_SHADER_STAGE_VERTEX_INDEX] = graphicsPipeline->vertexShaderModule();
    activeShaderModules[LV_SHADER_STAGE_FRAGMENT_INDEX] = graphicsPipeline->fragmentShaderModule();
}

void Metal_CommandBuffer::cmdPushConstants(void* data, uint16_t index) {
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

void Metal_CommandBuffer::cmdBindViewport(Metal_Viewport* viewport) {
    //TODO: fix this
    //[encoder setViewport:viewport];
    //[encoder setScissorRect:scissor];
}

void Metal_CommandBuffer::cmdBindTessellationFactorBuffer(Metal_Buffer* buffer) {
    [encoder setTessellationFactorBuffer:buffer->buffer(g_metal_swapChain->crntFrame() % buffer->frameCount()) offset:0 instanceStride:0];
}

void Metal_CommandBuffer::cmdBindTessellationFactorBufferForWriting(Metal_Buffer* buffer) {
    [encoder setBuffer:buffer->buffer(g_metal_swapChain->crntFrame() % buffer->frameCount())
                offset:0
               atIndex:26]; //TODO: check whether it's always 26
}

void Metal_CommandBuffer::cmdEnableWireframeMode() {
    [encoder setTriangleFillMode:MTLTriangleFillModeLines];
}

//Compute
void Metal_CommandBuffer::cmdBindComputePipeline(Metal_ComputePipeline* computePipeline) {
    [encoder setComputePipelineState:computePipeline->computePipeline()];
    activePipelineLayout = computePipeline->pipelineLayout();
    activeShaderModules[LV_SHADER_STAGE_COMPUTE_INDEX] = computePipeline->computeShaderModule();
}

void Metal_CommandBuffer::cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ, uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ) {
    [encoder dispatchThreadgroups:MTLSizeMake(threadgroupsX, threadgroupsY, threadgroupsZ)
            threadsPerThreadgroup:MTLSizeMake(threadsPerGroupX, threadsPerGroupY, threadsPerGroupZ)];
}

void Metal_CommandBuffer::cmdDispatchTessellationControl(uint32_t patchCount) {
    [encoder dispatchThreadgroups:MTLSizeMake(patchCount, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(1, 1, 1)]; //TODO: add option to dispatch for every control point
}

//Blit
void Metal_CommandBuffer::cmdStagingCopyDataToBuffer(Metal_Buffer* buffer, void* data, size_t aSize) {
    if (aSize == 0)
        aSize = buffer->size();
    uint8_t index = g_metal_swapChain->crntFrame() % buffer->frameCount();
    
    id<MTLBuffer> stagingBuffer = [g_metal_device->device() newBufferWithLength:aSize
                                                                        options:MTLResourceStorageModeShared];
    memcpy([stagingBuffer contents], data, aSize);

    Metal_Buffer::copyBufferToBuffer(encoder, stagingBuffer, buffer->buffer(index), aSize);

    [stagingBuffer release];
}

void Metal_CommandBuffer::cmdStagingCopyDataToImage(Metal_Image* image, void* data, uint8_t bytesPerPixel) {
    size_t size = image->width() * image->height() * bytesPerPixel;
    id<MTLBuffer> stagingBuffer = [g_metal_device->device() newBufferWithLength:size
                                                                      options:MTLResourceStorageModeShared];
    memcpy([stagingBuffer contents], data, size);

    for (uint8_t i = 0; i < image->frameCount(); i++)
        Metal_Buffer::copyBufferToImage(encoder, stagingBuffer, image->image(i), image->width(), image->height(), bytesPerPixel);

    [stagingBuffer release];
}

void Metal_CommandBuffer::cmdGenerateMipmapsForImage(Metal_Image* image, uint8_t aFrameCount) {
    if (aFrameCount == 0) aFrameCount = image->frameCount();

    for (uint8_t i = 0; i < aFrameCount; i++)
        [encoder generateMipmapsForTexture:image->image(g_metal_swapChain->crntFrame() + i)];
}

void Metal_CommandBuffer::cmdCopyToImageFromImage(Metal_Image* source, Metal_Image* destination) {
    [encoder copyFromTexture:source->image(g_metal_swapChain->crntFrame() % source->frameCount())
                              toTexture:destination->image(g_metal_swapChain->crntFrame() % destination->frameCount())];
}

void Metal_CommandBuffer::cmdBlitToImageFromImage(Metal_Image* source, Metal_Image* destination) {
    if (!Metal_Image::blitComputePipelineState) {
        std::string source = readFile("/Users/samuliak/Documents/lava_core/src/metal/blit.metallib"); //TODO: load this file independently of path

        dispatch_data_t sourceData = dispatch_data_create((void*)source.c_str(), source.size() * sizeof(char), nullptr, nullptr);

        //Library
        NSError* error;
        Metal_Image::blitComputeLibrary = [g_metal_device->device() newLibraryWithData:sourceData
                                                    error:&error];
        if (!Metal_Image::blitComputeLibrary) {
            throw std::runtime_error([[error localizedDescription] UTF8String]);
        }

        //Function
        Metal_Image::blitComputeFunction = [Metal_Image::blitComputeLibrary newFunctionWithName:@"main0"];

        //Pipeline state
        MTLComputePipelineDescriptor* computePipelineDesc = [[MTLComputePipelineDescriptor alloc] init];
        computePipelineDesc.threadGroupSizeIsMultipleOfThreadExecutionWidth = true;
        computePipelineDesc.computeFunction = Metal_Image::blitComputeFunction;
        
        Metal_Image::blitComputePipelineState = [g_metal_device->device() newComputePipelineStateWithDescriptor:computePipelineDesc
                                                                                         options:0
                                                                                      reflection:nullptr
                                                                                           error:&error];
        if (!Metal_Image::blitComputePipelineState) {
            throw std::runtime_error([[error localizedDescription] UTF8String]);
        }

        [computePipelineDesc release];
    }

    [encoder setComputePipelineState:Metal_Image::blitComputePipelineState];

    [encoder setTexture:source->image(g_metal_swapChain->crntFrame() % source->frameCount())
                atIndex:0];
    [encoder setTexture:destination->image(g_metal_swapChain->crntFrame() % destination->frameCount())
                atIndex:1];

    uint8_t threadGroupSize = 32;
    [encoder dispatchThreadgroups:MTLSizeMake(threadGroupSize, threadGroupSize, 1)
            threadsPerThreadgroup:MTLSizeMake((destination->width() + threadGroupSize - 1) / threadGroupSize, (destination->height() + threadGroupSize - 1) / threadGroupSize, 1)];
}

//Other
void Metal_CommandBuffer::cmdPresent() {
    [_getActiveCommandBuffer() presentDrawable:g_metal_swapChain->drawable()];
}

} //namespace lv
