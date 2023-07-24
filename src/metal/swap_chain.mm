#include "metal/lvcore/swap_chain.hpp"

#import <QuartzCore/QuartzCore.h>

#include "metal/lvcore/device.hpp"

namespace lv {

namespace metal {

SwapChain* g_metal_swapChain = nullptr;

SwapChain::SwapChain(internal::SwapChainCreateInfo createInfo) {
    g_metal_swapChain = this;

    _maxFramesInFlight = createInfo.maxFramesInFlight;
    loadOp = (createInfo.clearAttachment ? AttachmentLoadOperation::Clear : AttachmentLoadOperation::DontCare);
    hasDepthAttachment = createInfo.createDepthAttachment;
    _window = createInfo.window;

    _commandBuffer = new CommandBuffer({});

    create();

    semaphore = new Semaphore();

    //Render pass
    colorImage = (Image*)malloc(sizeof(Image));
    colorImage->_setFrameCount(_maxFramesInFlight);
    colorImage->_setWidth(_width);
    colorImage->_setHeight(_height);
    colorImage->_setFormat(Format::BGRA8Unorm_sRGB);

    internal::SubpassCreateInfo subpassCreateInfo{};
    subpassCreateInfo.colorAttachments = {{0}};

    if (hasDepthAttachment)
        subpassCreateInfo.depthAttachment = {1};
    
    _subpass = new Subpass(subpassCreateInfo);

    internal::RenderPassCreateInfo renderPassCreateInfo{};
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

    _renderPass = new RenderPass(renderPassCreateInfo);

    _framebuffer = new Framebuffer({
        .renderPass = _renderPass,
        .colorAttachments = {
            {0, colorImage}
        }
    });
}

SwapChain::~SwapChain() {
    delete _framebuffer;
    delete _renderPass;
    delete depthImage;
    delete semaphore;
}

void SwapChain::create() {
    float xscale, yscale;
    lvndWindowGetFramebufferScale(_window, &xscale, &yscale);

    uint16_t width, height;
    lvndWindowGetFramebufferSize(_window, &width, &height);

    lvndMetalCreateLayer(_window, width, height, g_metal_device->device());

    _width = width;
    _height = height;

    //Depth attachment
    if (hasDepthAttachment) {
        depthImage = new Image({
            .format = Format::D32Float,
            .width = width,
            .height = height,
            .usage = ImageUsageFlags::DepthStencilAttachment,
            .aspect = ImageAspectFlags::Depth
        });
    }
}

void SwapChain::resize() {
    delete depthImage;
    create();
}

void SwapChain::acquireNextImage() {
    _drawable = reinterpret_cast<id<CAMetalDrawable> >(lvndMetalNextDrawable(_window));
    if (_drawable == nullptr) {
        throw std::runtime_error("Failed to acquire next drawable");
    }

    colorImage->_setImage([_drawable texture], _crntFrame);
    MTLRenderPassColorAttachmentDescriptor* colorAttachment = ((MTLRenderPassDescriptor*)_framebuffer->renderPass(_crntFrame)).colorAttachments[0];
    colorAttachment.texture = colorImage->image(_crntFrame);
}

void SwapChain::renderAndPresent() {
    _commandBuffer->cmdPresent();
    _commandBuffer->submit(nullptr, semaphore);

    [_drawable release];

    _crntFrame = (_crntFrame + 1) % _maxFramesInFlight;
}

} //namespace metal

} //namespace lv
