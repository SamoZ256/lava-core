#include "lvcore/vulkan/framebuffer.hpp"

#include <cmath>

#include "lvcore/vulkan/swap_chain.hpp"

namespace lv {

namespace vulkan {

Framebuffer::Framebuffer(internal::FramebufferCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

    _renderPass = static_cast<RenderPass*>(createInfo.renderPass);

    /*
    bool hasDepthAttachment = (depthAttachment.attachmentIndex != -1);
    for (auto& attachment : colorAttachments) {
        if (attachment.image == nullptr)
            throw std::invalid_argument("Color attachment number " + std::to_string(attachment.attachmentIndex) + " has invalid image");
        if (attachment.imageView == nullptr)
            throw std::invalid_argument("Color attachment number " + std::to_string(attachment.attachmentIndex) + " has invalid image view");
        if (attachment.attachmentIndex == -1)
            throw std::invalid_argument("Color attachment has invalid attachment index");
    }
    if (hasDepthAttachment) {
        if (depthAttachment.image == nullptr)
            throw std::invalid_argument("Depth attachment has invalid image");
        if (depthAttachment.imageView == nullptr)
            throw std::invalid_argument("Depth attachment has invalid image view");
        if (depthAttachment.attachmentIndex == -1)
            throw std::invalid_argument("Depth attachment has invalid attachment index");
    }
    */

    for (auto& colorAttachment : createInfo.colorAttachments) {
        maxLayerCount = std::max(colorAttachment.image->layerCount(), maxLayerCount);
        uint16_t scale = pow(2, colorAttachment.image->baseMip());
        _width = colorAttachment.image->width() / scale;
        _height = colorAttachment.image->height() / scale;
    }
    if (createInfo.depthAttachment.index != -1) {
        maxLayerCount = std::max((uint16_t)createInfo.depthAttachment.image->layerCount(), maxLayerCount);
        uint16_t scale = pow(2, createInfo.depthAttachment.image->baseMip());
        _width = createInfo.depthAttachment.image->width() / scale;
        _height = createInfo.depthAttachment.image->height() / scale;
    }

    //Framebuffer
    framebuffers.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        std::vector<VkImageView> imageViews;
        for (auto& colorAttachment : createInfo.colorAttachments) {
            CAST_FROM_INTERNAL_NAMED(colorAttachment.image, Image, image);
            imageViews.push_back(image->imageView(i));
        }
        if (createInfo.depthAttachment.index != -1) {
            CAST_FROM_INTERNAL_NAMED(createInfo.depthAttachment.image, Image, image);
            imageViews.push_back(image->imageView(i));
        }

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass->renderPass();
        framebufferInfo.attachmentCount = imageViews.size();
        framebufferInfo.pAttachments = imageViews.data();
        framebufferInfo.width = _width;
        framebufferInfo.height = _height;
        framebufferInfo.layers = maxLayerCount;

        VK_CHECK_RESULT(vkCreateFramebuffer(g_vulkan_device->device(), &framebufferInfo, nullptr, &framebuffers[i]))
    }

    //Clear values
    clearValues.resize(createInfo.colorAttachments.size() + (createInfo.depthAttachment.index == -1 ? 0 : 1));
    for (auto& colorAttachment : createInfo.colorAttachments)
        clearValues[colorAttachment.index].color = {colorAttachment.clearValue.color.float32[0], colorAttachment.clearValue.color.float32[1], colorAttachment.clearValue.color.float32[2], colorAttachment.clearValue.color.float32[3]};

    if (createInfo.depthAttachment.index != -1)
        clearValues[createInfo.depthAttachment.index].depthStencil = {createInfo.depthAttachment.clearValue.depthStencil.depth, createInfo.depthAttachment.clearValue.depthStencil.stencil};
}

Framebuffer::~Framebuffer() {
    for (auto& framebuffer : framebuffers) {
        vkDestroyFramebuffer(g_vulkan_device->device(), framebuffer, nullptr);
    }
}

/*
void Framebuffer::resize(uint16_t aWidth, uint16_t aHeight) {
    for (auto& attachment : colorAttachments) {
        if (!attachment->image->resized) {
            attachment.image->resize(aWidth, aHeight);
            //std::cout << attachment.image->resized << std::endl;
            attachment.image->resized = true;
            //std::cout << attachment.image->resized << std::endl;
        }
        attachment.imageView->init(attachment.image);
    }
    for (auto& attachment : colorAttachments) {
        attachment.image->resized = false;
    }
    if (depthAttachment.attachmentIndex != -1) depthAttachment.image->resize(aWidth, aHeight);
    destroyToRecreate();
    renderPass->init(getAttachmentDescriptions());
    init(renderPass, aWidth, aHeight);
}
*/

} //namespace vulkan

} //namespace lv
