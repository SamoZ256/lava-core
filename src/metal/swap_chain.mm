#include "metal/lvcore/core/swap_chain.hpp"

#import <QuartzCore/QuartzCore.h>

#include "metal/lvcore/core/device.hpp"

namespace lv {

Metal_SwapChain* g_metal_swapChain = nullptr;

Metal_SwapChain::Metal_SwapChain(Metal_SwapChainCreateInfo createInfo) {
    g_metal_swapChain = this;

    _maxFramesInFlight = createInfo.maxFramesInFlight;
    loadOp = (createInfo.clearAttachment ? AttachmentLoadOperation::Clear : AttachmentLoadOperation::DontCare);
    hasDepthAttachment = createInfo.createDepthAttachment;
    _window = createInfo.window;

    _commandBuffer = new Metal_CommandBuffer({});

    create();

    semaphore = new Metal_Semaphore();

    //Render pass
    colorImage = (Metal_Image*)malloc(sizeof(Metal_Image));
    colorImage->_setFrameCount(_maxFramesInFlight);
    colorImage->_setWidth(_width);
    colorImage->_setHeight(_height);
    colorImage->_setFormat(Format::BGRA8Unorm_sRGB);

    Metal_SubpassCreateInfo subpassCreateInfo{};
    subpassCreateInfo.colorAttachments = {{0}};

    if (hasDepthAttachment)
        subpassCreateInfo.depthAttachment = {1};
    
    _subpass = new Metal_Subpass(subpassCreateInfo);

    Metal_RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.subpasses = {_subpass};
    renderPassCreateInfo.attachments = {
        {
            .format = colorImage->format(),
            .index = 0,
            .loadOp = loadOp,
            .storeOp = AttachmentStoreOperation::Store
        }
    };

    if (hasDepthAttachment) {
        renderPassCreateInfo.attachments.push_back({
            .format = depthImage->format(),
            .index = 1,
            .loadOp = AttachmentLoadOperation::Clear,
            .storeOp = AttachmentStoreOperation::DontCare
        });
    }

    _renderPass = new Metal_RenderPass(renderPassCreateInfo);

    _framebuffer = new Metal_Framebuffer({
        .renderPass = _renderPass,
        .colorAttachments = {
            {0, colorImage}
        }
    });
}

Metal_SwapChain::~Metal_SwapChain() {
    delete _framebuffer;
    delete _renderPass;
    delete depthImage;
    delete semaphore;
}

void Metal_SwapChain::create() {
    float xscale, yscale;
    lvndWindowGetFramebufferScale(_window, &xscale, &yscale);

    uint16_t width, height;
    lvndWindowGetFramebufferSize(_window, &width, &height);

    lvndMetalCreateLayer(_window, width, height, g_metal_device->device());

    _width = width;
    _height = height;

    //Depth attachment
    if (hasDepthAttachment) {
        depthImage = new Metal_Image({
            .format = Format::D32Float,
            .width = width,
            .height = height,
            .usage = ImageUsageFlags::DepthStencilAttachment,
            .aspect = ImageAspectFlags::Depth
        });
    }
}

void Metal_SwapChain::resize() {
    delete depthImage;
    create();
}

void Metal_SwapChain::acquireNextImage() {
    _drawable = reinterpret_cast<id<CAMetalDrawable> >(lvndMetalNextDrawable(_window));
    if (_drawable == nullptr) {
        throw std::runtime_error("Failed to acquire next drawable");
    }

    colorImage->_setImage([_drawable texture], _crntFrame);
    MTLRenderPassColorAttachmentDescriptor* colorAttachment = ((MTLRenderPassDescriptor*)_framebuffer->renderPass(_crntFrame)).colorAttachments[0];
    colorAttachment.texture = colorImage->image(_crntFrame);
}

void Metal_SwapChain::renderAndPresent() {
    _commandBuffer->cmdPresent();
    _commandBuffer->submit(nullptr, semaphore);

    [_drawable release];

    _crntFrame = (_crntFrame + 1) % _maxFramesInFlight;
}

} //namespace lv