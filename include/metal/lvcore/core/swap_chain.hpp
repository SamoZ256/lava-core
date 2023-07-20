#ifndef LV_METAL_SWAP_CHAIN_H
#define LV_METAL_SWAP_CHAIN_H

#include <stdexcept>

#define LVND_BACKEND_METAL
#include "lvnd/lvnd.h"

#include "command_buffer.hpp"

namespace lv {

struct Metal_SwapChainCreateInfo {
	LvndWindow* window;
  	Bool vsyncEnable = True;
	uint8_t maxFramesInFlight = 2;
	Bool clearAttachment = False;
    Bool createDepthAttachment = False;
};

class Metal_SwapChain {
private:
    uint8_t _maxFramesInFlight;
    uint8_t _crntFrame = 0;

    uint32_t _width;
    uint32_t _height;

    lv::AttachmentLoadOperation loadOp;

    Bool hasDepthAttachment;

    LvndWindow* _window;
    id /*CAMetalDrawable*/ _drawable;
    Metal_Semaphore* semaphore;

    Metal_Image* colorImage;
    Metal_Image* depthImage;

    Metal_Framebuffer* _framebuffer;
    Metal_CommandBuffer* _commandBuffer;
    Metal_Subpass* _subpass;
    Metal_RenderPass* _renderPass;

public:
    Metal_SwapChain(Metal_SwapChainCreateInfo createInfo);

    ~Metal_SwapChain();

    void create();

    void resize();

    void acquireNextImage();

    void renderAndPresent();

    //Getters
    inline uint8_t maxFramesInFlight() { return _maxFramesInFlight; }

    inline uint8_t crntFrame() { return _crntFrame; }

    inline uint32_t width() { return _width; }

    inline uint32_t height() { return _height; }

    inline id /*CAMetalDrawable*/ drawable() { return _drawable; }

    inline Metal_Framebuffer* framebuffer() { return _framebuffer; }

    inline Metal_CommandBuffer* commandBuffer() { return _commandBuffer; }

    inline Metal_Subpass* subpass() { return _subpass; }

    inline Metal_RenderPass* renderPass() { return _renderPass; }
};

extern Metal_SwapChain* g_metal_swapChain;

} //namespace lv

#endif
