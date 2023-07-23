#include "metal/lvcore/core/framebuffer.hpp"

#include "metal/lvcore/core/core.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

#include <iostream>

namespace lv {

Metal_Framebuffer::Metal_Framebuffer(Metal_FramebufferCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);

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
            //Metal_SubpassAttachment& subpassAttachment = subpass->colorAttachments[j];
            Metal_FramebufferAttachment& framebufferAttachment = createInfo.colorAttachments[j];
            Metal_RenderPassAttachment* renderPassAttachment = createInfo.renderPass->sortedAttachments[framebufferAttachment.index];
            MTLRenderPassColorAttachmentDescriptor* attachment = ((MTLRenderPassDescriptor*)renderPasses[i]).colorAttachments[j];
            //if (colorAttachments[i].clearValue.color.float32[0] != 0.0f)
            //    std::cout << "CLEAR COLOR X: " << colorAttachments[i].clearValue.color.float32[0] << std::endl;

            MTLLoadAction mtlLoadAction;
            GET_MTL_LOAD_ACTION(renderPassAttachment->loadOp, mtlLoadAction);
            MTLStoreAction mtlStoreAction;
            GET_MTL_STORE_ACTION(renderPassAttachment->storeOp, mtlStoreAction);

            attachment.clearColor = MTLClearColorMake(createInfo.colorAttachments[j].clearValue.color.float32[0], createInfo.colorAttachments[j].clearValue.color.float32[1], createInfo.colorAttachments[j].clearValue.color.float32[2], createInfo.colorAttachments[j].clearValue.color.float32[3]);
            attachment.loadAction = mtlLoadAction;
            attachment.storeAction = mtlStoreAction;
            attachment.texture = framebufferAttachment.image->image(i % framebufferAttachment.image->frameCount());
            maxArrayLength = std::max(maxArrayLength, uint16_t(framebufferAttachment.image->layerCount() * framebufferAttachment.image->layersPerLayer()));
            //std::cout << "Index: " << (int)subpassAttachment.index << std::endl;
            //std::cout << "Color attachment " << (int)j << ": " << framebufferAttachment.image->format << std::endl;
        }

        if (createInfo.depthAttachment.index != -1) {
            //Metal_SubpassAttachment& subpassAttachment = subpass->depthAttachment;
            Metal_RenderPassAttachment* renderPassAttachment = createInfo.renderPass->sortedAttachments[createInfo.depthAttachment.index];

            MTLLoadAction mtlLoadAction;
            GET_MTL_LOAD_ACTION(renderPassAttachment->loadOp, mtlLoadAction);
            MTLStoreAction mtlStoreAction;
            GET_MTL_STORE_ACTION(renderPassAttachment->storeOp, mtlStoreAction);

            MTLRenderPassDepthAttachmentDescriptor* attachment = ((MTLRenderPassDescriptor*)renderPasses[i]).depthAttachment;
            attachment.clearDepth = createInfo.depthAttachment.clearValue.depthStencil.depth;
            attachment.loadAction = mtlLoadAction;
            attachment.storeAction = mtlStoreAction;
            attachment.texture = createInfo.depthAttachment.image->image(i % createInfo.depthAttachment.image->frameCount());
            maxArrayLength = std::max(maxArrayLength, uint16_t(createInfo.depthAttachment.image->layerCount() * createInfo.depthAttachment.image->layersPerLayer()));
        }

        ((MTLRenderPassDescriptor*)renderPasses[i]).renderTargetArrayLength = maxArrayLength;
    }
        //subpasses[subpassIndex] = framebufferSubpass;
    //}
}

Metal_Framebuffer::~Metal_Framebuffer() {
    //for (auto& subpass : subpasses) {
    for (uint8_t i = 0; i < _frameCount; i++) {
        [(MTLRenderPassDescriptor*)renderPasses[i] release];
    }
    //}
}

/*
void Metal_Framebuffer::nextSubpass() {
    _endSubpass();
    crntSubpass++;
    _beginSubpass();
}

void Metal_Framebuffer::_beginSubpass() {
    
}

void Metal_Framebuffer::_endSubpass() {
    
}
*/

} //namespace lv
