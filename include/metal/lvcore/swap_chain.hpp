#ifndef LV_METAL_SWAP_CHAIN_H
#define LV_METAL_SWAP_CHAIN_H

#include <stdexcept>

#include "lvcore/internal/swap_chain.hpp"

#include "command_buffer.hpp"

namespace lv {

namespace metal {

class SwapChain : public internal::SwapChain {
private:
    uint32_t _width;
    uint32_t _height;

    AttachmentLoadOperation loadOp;

    Bool hasDepthAttachment;

    LvndWindow* _window;
    id /*CAMetalDrawable*/ _drawable;
    Semaphore* semaphore;

    Image* colorImage;
    Image* depthImage;

    Framebuffer* _framebuffer;
    CommandBuffer* _commandBuffer;
    Subpass* _subpass;
    RenderPass* _renderPass;

public:
    SwapChain(internal::SwapChainCreateInfo createInfo);

    ~SwapChain() override;

    void create();

    void resize() override;

    void acquireNextImage() override;

    void renderAndPresent() override;

    //Getters
    inline uint32_t width() { return _width; }

    inline uint32_t height() { return _height; }

    inline id /*CAMetalDrawable*/ drawable() { return _drawable; }

    inline Framebuffer* framebuffer() { return _framebuffer; }

    inline CommandBuffer* commandBuffer() { return _commandBuffer; }

    inline Subpass* subpass() { return _subpass; }

    inline RenderPass* renderPass() { return _renderPass; }
};

extern SwapChain* g_metal_swapChain;

} //namespace metal

} //namespace lv

#endif
