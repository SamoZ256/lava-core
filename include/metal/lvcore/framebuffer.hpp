#ifndef LV_METAL_FRAMEBUFFER_H
#define LV_METAL_FRAMEBUFFER_H

#include "lvcore/internal/framebuffer.hpp"

namespace lv {

namespace metal {

class Framebuffer : public internal::Framebuffer {
private:
    std::vector<void* /*MTLRenderPassDescriptor*/> renderPasses;

public:
    Framebuffer(internal::FramebufferCreateInfo createInfo);

    ~Framebuffer() override;

    //Getters
    inline void* /*MTLRenderPassDescriptor*/ renderPass(uint8_t index) { return renderPasses[index]; }
};

} //namespace metal

} //namespace lv

#endif
