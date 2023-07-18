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
            attachment.blendingEnabled = true;
            attachment.sourceRGBBlendFactor = (MTLBlendFactor)createInfo.colorBlendAttachments[i].srcRgbBlendFactor;
            attachment.destinationRGBBlendFactor = (MTLBlendFactor)createInfo.colorBlendAttachments[i].dstRgbBlendFactor;
            attachment.rgbBlendOperation = (MTLBlendOperation)createInfo.colorBlendAttachments[i].rgbBlendOp;
            attachment.sourceAlphaBlendFactor = (MTLBlendFactor)createInfo.colorBlendAttachments[i].srcAlphaBlendFactor;
            attachment.destinationAlphaBlendFactor = (MTLBlendFactor)createInfo.colorBlendAttachments[i].dstAlphaBlendFactor;
            attachment.alphaBlendOperation = (MTLBlendOperation)createInfo.colorBlendAttachments[i].alphaBlendOp;
        }
    }

    descriptor.tessellationFactorScaleEnabled = NO;
    descriptor.tessellationFactorFormat = MTLTessellationFactorFormatHalf;
    descriptor.tessellationControlPointIndexType = MTLTessellationControlPointIndexTypeNone;
    descriptor.tessellationFactorStepFunction = MTLTessellationFactorStepFunctionPerPatch;
    descriptor.tessellationOutputWindingOrder = (MTLWinding)createInfo.windingOrder;
    descriptor.tessellationPartitionMode = (MTLTessellationPartitionMode)createInfo.tessellationSpacing;
    descriptor.maxTessellationFactor = createInfo.maxTessellationFactor;

    _LV_CREATE_GRAPHICS_PIPELINE;

    MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
    if (createInfo.depthTestEnable) {
        depthStencilDesc.depthCompareFunction = (MTLCompareFunction)createInfo.depthOp;
        depthStencilDesc.depthWriteEnabled = createInfo.depthWriteEnable;
    }

    _depthStencilState = [g_metal_device->device() newDepthStencilStateWithDescriptor:depthStencilDesc];
}

void Metal_GraphicsPipeline::recompile() {
    [_graphicsPipeline release];

    _LV_CREATE_GRAPHICS_PIPELINE;
}

} //namespace lv
