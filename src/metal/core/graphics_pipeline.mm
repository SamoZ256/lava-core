#include "metal/lvcore/core/graphics_pipeline.hpp"

#include <string>
#include <iostream>

#include "metal/lvcore/core/core.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

#define _LV_CREATE_GRAPHICS_PIPELINE \
NSError* error; \
_graphicsPipeline = [g_metal_device->device() newRenderPipelineStateWithDescriptor:descriptor \
                                                                            error:&error]; \
if (!_graphicsPipeline) { \
    throw std::runtime_error("Failed to create graphics pipeline: " + std::string([[error localizedDescription] UTF8String])); \
}

Metal_GraphicsPipeline::Metal_GraphicsPipeline(Metal_GraphicsPipelineCreateInfo createInfo) {
    _vertexShaderModule = createInfo.vertexShaderModule;
    _fragmentShaderModule = createInfo.fragmentShaderModule;
    _pipelineLayout = createInfo.pipelineLayout;
    _cullMode = createInfo.cullMode;

    compile(createInfo);
}

Metal_GraphicsPipeline::~Metal_GraphicsPipeline() {
    [descriptor release];
    [_graphicsPipeline release];
}

void Metal_GraphicsPipeline::compile(Metal_GraphicsPipelineCreateInfo& createInfo) {
    MTLCompareFunction mtlCompareFunction;
    GET_MTL_COMPARE_FUNCTION(createInfo.depthOp, mtlCompareFunction);

    descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.vertexFunction = _vertexShaderModule->function();
    descriptor.fragmentFunction = _fragmentShaderModule->function();
    if (createInfo.vertexDescriptor)
        descriptor.vertexDescriptor = (MTLVertexDescriptor*)createInfo.vertexDescriptor->descriptor();
    
    descriptor.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;

    Metal_Subpass* subpass = createInfo.renderPass->subpasses[createInfo.subpassIndex];
    if (subpass->depthAttachment.index != -1) {
        for (uint8_t i = 0; i < createInfo.renderPass->attachments.size(); i++) {
            MTLPixelFormat format;
            GET_MTL_PIXEL_FORMAT(createInfo.renderPass->attachments[i].format, format);
            if (format >= LV_FORMAT_D16_UNORM && format <= MTLPixelFormatX24_Stencil8)
                descriptor.depthAttachmentPixelFormat = format;
        }
    }

    //Setting blend states
    for (uint8_t i = 0; i < createInfo.colorBlendAttachments.size(); i++) {
        Metal_RenderPassAttachment* renderPassAttachment = &createInfo.renderPass->attachments[createInfo.colorBlendAttachments[i].index];
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

    descriptor.tessellationFactorScaleEnabled = NO;
    descriptor.tessellationFactorFormat = MTLTessellationFactorFormatHalf;
    descriptor.tessellationControlPointIndexType = MTLTessellationControlPointIndexTypeNone;
    descriptor.tessellationFactorStepFunction = MTLTessellationFactorStepFunctionPerPatch;
    descriptor.tessellationOutputWindingOrder = (MTLWinding)createInfo.windingOrder;
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

void Metal_GraphicsPipeline::recompile() {
    [_graphicsPipeline release];

    _LV_CREATE_GRAPHICS_PIPELINE;
}

} //namespace lv
