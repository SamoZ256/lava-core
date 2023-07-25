#include "lvcore/metal/graphics_pipeline.hpp"

#include <string>
#include <iostream>

#include "lvcore/metal/core.hpp"

#include "lvcore/metal/device.hpp"
#include "lvcore/metal/swap_chain.hpp"

namespace lv {

namespace metal {

#define _LV_CREATE_GRAPHICS_PIPELINE \
NSError* error; \
_graphicsPipeline = [g_metal_device->device() newRenderPipelineStateWithDescriptor:descriptor \
                                                                            error:&error]; \
if (!_graphicsPipeline) { \
    throw std::runtime_error("Failed to create graphics pipeline: " + std::string([[error localizedDescription] UTF8String])); \
}

GraphicsPipeline::GraphicsPipeline(internal::GraphicsPipelineCreateInfo createInfo) {
    _vertexShaderModule = (ShaderModule*)createInfo.vertexShaderModule;
    _fragmentShaderModule = (ShaderModule*)createInfo.fragmentShaderModule;
    _pipelineLayout = (PipelineLayout*)createInfo.pipelineLayout;
    _cullMode = createInfo.cullMode;

    compile(createInfo);
}

GraphicsPipeline::~GraphicsPipeline() {
    [descriptor release];
    [_graphicsPipeline release];
}

void GraphicsPipeline::compile(internal::GraphicsPipelineCreateInfo& createInfo) {
    CAST_FROM_INTERNAL_NAMED(createInfo.renderPass, RenderPass, renderPass);
    CAST_FROM_INTERNAL_NAMED(createInfo.vertexDescriptor, VertexDescriptor, vertexDescriptor);
    CAST_FROM_INTERNAL_NAMED(renderPass->subpasses[createInfo.subpassIndex], Subpass, subpass);

    MTLCompareFunction mtlCompareFunction;
    GET_MTL_COMPARE_FUNCTION(createInfo.depthOp, mtlCompareFunction);

    descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.vertexFunction = _vertexShaderModule->function();
    descriptor.fragmentFunction = _fragmentShaderModule->function();
    if (createInfo.vertexDescriptor)
        descriptor.vertexDescriptor = (MTLVertexDescriptor*)vertexDescriptor->descriptor();
    
    descriptor.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;

    if (subpass->depthAttachment.index != -1) {
        for (uint8_t i = 0; i < renderPass->attachments.size(); i++) {
            MTLPixelFormat format;
            GET_MTL_PIXEL_FORMAT(renderPass->attachments[i].format, format);
            if (format >= MTLPixelFormatDepth16Unorm && format <= MTLPixelFormatX24_Stencil8)
                descriptor.depthAttachmentPixelFormat = format;
        }
    }

    //Setting blend states
    for (uint8_t i = 0; i < createInfo.colorBlendAttachments.size(); i++) {
        internal::RenderPassAttachment* renderPassAttachment = &renderPass->attachments[createInfo.colorBlendAttachments[i].index];
        MTLRenderPipelineColorAttachmentDescriptor* attachment = descriptor.colorAttachments[i];
        MTLPixelFormat format;
        GET_MTL_PIXEL_FORMAT(renderPassAttachment->format, format);
        attachment.pixelFormat = format;
        if (createInfo.colorBlendAttachments[i].blendEnable) {
            MTLBlendFactor sourceRGBMTLBlendFactor;
            GET_MTL_BLEND_FACTOR(createInfo.colorBlendAttachments[i].srcRgbBlendFactor, sourceRGBMTLBlendFactor);
            MTLBlendFactor destinationRGBMTLBlendFactor;
            GET_MTL_BLEND_FACTOR(createInfo.colorBlendAttachments[i].dstRgbBlendFactor, destinationRGBMTLBlendFactor);
            MTLBlendOperation rgbMTLBlendOperation;
            GET_MTL_BLEND_OPERATION(createInfo.colorBlendAttachments[i].rgbBlendOp, rgbMTLBlendOperation);

            MTLBlendFactor sourceAlphaMTLBlendFactor;
            GET_MTL_BLEND_FACTOR(createInfo.colorBlendAttachments[i].srcAlphaBlendFactor, sourceAlphaMTLBlendFactor);
            MTLBlendFactor destinationAlphaMTLBlendFactor;
            GET_MTL_BLEND_FACTOR(createInfo.colorBlendAttachments[i].dstAlphaBlendFactor, destinationAlphaMTLBlendFactor);
            MTLBlendOperation alphaMTLBlendOperation;
            GET_MTL_BLEND_OPERATION(createInfo.colorBlendAttachments[i].alphaBlendOp, alphaMTLBlendOperation);

            attachment.blendingEnabled = true;
            attachment.sourceRGBBlendFactor = sourceRGBMTLBlendFactor;
            attachment.destinationRGBBlendFactor = destinationRGBMTLBlendFactor;
            attachment.rgbBlendOperation = rgbMTLBlendOperation;
            attachment.sourceAlphaBlendFactor = sourceAlphaMTLBlendFactor;
            attachment.destinationAlphaBlendFactor = destinationAlphaMTLBlendFactor;
            attachment.alphaBlendOperation = alphaMTLBlendOperation;
        }
    }

    MTLTessellationPartitionMode mtlTessellationPartitionMode;
    GET_MTL_TESSELLATION_PARTITION_MODE(createInfo.tessellationSpacing, mtlTessellationPartitionMode);
    MTLWinding mtlWinding;
    GET_MTL_WINDING(createInfo.frontFace, mtlWinding);

    descriptor.tessellationFactorScaleEnabled = NO;
    descriptor.tessellationFactorFormat = MTLTessellationFactorFormatHalf;
    descriptor.tessellationControlPointIndexType = MTLTessellationControlPointIndexTypeNone;
    descriptor.tessellationFactorStepFunction = MTLTessellationFactorStepFunctionPerPatch;
    descriptor.tessellationOutputWindingOrder = mtlWinding;
    descriptor.tessellationPartitionMode = mtlTessellationPartitionMode;
    descriptor.maxTessellationFactor = createInfo.maxTessellationFactor;

    _LV_CREATE_GRAPHICS_PIPELINE;

    MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
    if (createInfo.depthTestEnable) {
        depthStencilDesc.depthCompareFunction = mtlCompareFunction;
        depthStencilDesc.depthWriteEnabled = (bool)createInfo.depthWriteEnable;
    }

    _depthStencilState = [g_metal_device->device() newDepthStencilStateWithDescriptor:depthStencilDesc];
}

void GraphicsPipeline::recompile() {
    [_graphicsPipeline release];

    _LV_CREATE_GRAPHICS_PIPELINE;
}

} //namespace metal

} //namespace lv
