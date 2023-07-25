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

    Subpass* subpass;
    RenderPass* renderPass;
    Framebuffer* framebuffer;
    CommandBuffer* commandBuffer;

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

    inline Subpass* getSubpass() { return subpass; }

    internal::RenderPass* getRenderPass() override {
        return renderPass;
    }

    internal::Framebuffer* getFramebuffer() override {
        return framebuffer;
    }

    internal::CommandBuffer* getCommandBuffer() override {
        return commandBuffer;
    }
};

extern SwapChain* g_metal_swapChain;

} //namespace metal

} //namespace lv

#endif
