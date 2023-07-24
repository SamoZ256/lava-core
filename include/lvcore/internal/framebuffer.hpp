#ifndef LV_INTERNAL_FRAMEBUFFER_H
#define LV_INTERNAL_FRAMEBUFFER_H

#include "render_pass.hpp"

namespace lv {

namespace internal {

struct FramebufferCreateInfo {
    uint8_t frameCount = 0;
    RenderPass* renderPass;
    std::vector<FramebufferAttachment> colorAttachments;
    FramebufferAttachment depthAttachment;
};

class Framebuffer {
protected:
    uint8_t _frameCount;

    std::vector<FramebufferAttachment> colorAttachments;
    FramebufferAttachment depthAttachment;

public:
    virtual ~Framebuffer() {}

    //Getters
    inline uint8_t frameCount() { return _frameCount; }
};

} //namespace internal

} //namespace lv

#endif
