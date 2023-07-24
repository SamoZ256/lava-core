#include "metal/lvcore/framebuffer.hpp"

#include "metal/lvcore/core.hpp"

#include "metal/lvcore/device.hpp"
#include "metal/lvcore/swap_chain.hpp"

#include <iostream>

namespace lv {

namespace metal {

Framebuffer::Framebuffer(internal::FramebufferCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);

    CAST_FROM_INTERNAL_NAMED(createInfo.renderPass, RenderPass, renderPass);

    //sortedAttachments.resize(colorAttachments.size() + 1);
    //for (uint8_t i = 0; i < colorAttachments.size(); i++)
    //    sortedAttachments[colorAttachments[i].index] = &colorAttachments[i];
    
    //subpasses.resize(renderPass->subpasses.size());
    //for (uint8_t subpassIndex = 0; subpassIndex < subpasses.size(); subpassIndex++) {
    renderPasses.resize(_frameCount);
    //auto& subpass = renderPass->subpasses[subpassIndex];
    for (uint8_t i = 0; i < _frameCount; i++) {
        renderPasses[i] = [[MTLRenderPassDescriptor alloc] init];
        uint16_t maxArrayLength = 1;
        for (uint8_t j = 0; j < createInfo.colorAttachments.size(); j++) {
            //SubpassAttachment& subpassAttachment = subpass->colorAttachments[j];
            internal::FramebufferAttachment& framebufferAttachment = createInfo.colorAttachments[j];
            internal::RenderPassAttachment* renderPassAttachment = renderPass->sortedAttachments[framebufferAttachment.index];
            MTLRenderPassColorAttachmentDescriptor* attachment = ((MTLRenderPassDescriptor*)renderPasses[i]).colorAttachments[j];
            //if (colorAttachments[i].clearValue.color.float32[0] != 0.0f)
            //    std::cout << "CLEAR COLOR X: " << colorAttachments[i].clearValue.color.float32[0] << std::endl;

            CAST_FROM_INTERNAL_NAMED(framebufferAttachment.image, Image, image);

            MTLLoadAction mtlLoadAction;
            GET_MTL_LOAD_ACTION(renderPassAttachment->loadOp, mtlLoadAction);
            MTLStoreAction mtlStoreAction;
            GET_MTL_STORE_ACTION(renderPassAttachment->storeOp, mtlStoreAction);

            attachment.clearColor = MTLClearColorMake(createInfo.colorAttachments[j].clearValue.color.float32[0], createInfo.colorAttachments[j].clearValue.color.float32[1], createInfo.colorAttachments[j].clearValue.color.float32[2], createInfo.colorAttachments[j].clearValue.color.float32[3]);
            attachment.loadAction = mtlLoadAction;
            attachment.storeAction = mtlStoreAction;
            attachment.texture = image->image(i % framebufferAttachment.image->frameCount());
            maxArrayLength = std::max(maxArrayLength, uint16_t(framebufferAttachment.image->layerCount() * framebufferAttachment.image->layersPerLayer()));
            //std::cout << "Index: " << (int)subpassAttachment.index << std::endl;
            //std::cout << "Color attachment " << (int)j << ": " << framebufferAttachment.image->format << std::endl;
        }

        if (createInfo.depthAttachment.index != -1) {
            //SubpassAttachment& subpassAttachment = subpass->depthAttachment;
            internal::RenderPassAttachment* renderPassAttachment = renderPass->sortedAttachments[createInfo.depthAttachment.index];

            CAST_FROM_INTERNAL_NAMED(createInfo.depthAttachment.image, Image, image);

            MTLLoadAction mtlLoadAction;
            GET_MTL_LOAD_ACTION(renderPassAttachment->loadOp, mtlLoadAction);
            MTLStoreAction mtlStoreAction;
            GET_MTL_STORE_ACTION(renderPassAttachment->storeOp, mtlStoreAction);

            MTLRenderPassDepthAttachmentDescriptor* attachment = ((MTLRenderPassDescriptor*)renderPasses[i]).depthAttachment;
            attachment.clearDepth = createInfo.depthAttachment.clearValue.depthStencil.depth;
            attachment.loadAction = mtlLoadAction;
            attachment.storeAction = mtlStoreAction;
            attachment.texture = image->image(i % createInfo.depthAttachment.image->frameCount());
            maxArrayLength = std::max(maxArrayLength, uint16_t(createInfo.depthAttachment.image->layerCount() * createInfo.depthAttachment.image->layersPerLayer()));
        }

        ((MTLRenderPassDescriptor*)renderPasses[i]).renderTargetArrayLength = maxArrayLength;
    }
        //subpasses[subpassIndex] = framebufferSubpass;
    //}
}

Framebuffer::~Framebuffer() {
    //for (auto& subpass : subpasses) {
    for (uint8_t i = 0; i < _frameCount; i++) {
        [(MTLRenderPassDescriptor*)renderPasses[i] release];
    }
    //}
}

/*
void Framebuffer::nextSubpass() {
    _endSubpass();
    crntSubpass++;
    _beginSubpass();
}

void Framebuffer::_beginSubpass() {
    
}

void Framebuffer::_endSubpass() {
    
}
*/

} //namespace metal

} //namespace lv
