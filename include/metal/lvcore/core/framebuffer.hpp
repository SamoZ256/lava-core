#ifndef LV_METAL_FRAMEBUFFER_H
#define LV_METAL_FRAMEBUFFER_H

#include <vector>

#include "render_pass.hpp"

namespace lv {

//struct Metal_FramebufferSubpass {
//    std::vector<void* /*MTLRenderPassDescriptor*/> renderPasses;
//    id /*MTLRenderCommandEncoder*/ encoder = nullptr;
//};

struct Metal_FramebufferCreateInfo {
    uint8_t frameCount = 0;
    Metal_RenderPass* renderPass;
    std::vector<Metal_FramebufferAttachment> colorAttachments;
    Metal_FramebufferAttachment depthAttachment;
};

class Metal_Framebuffer {
private:
    uint8_t _frameCount;

    //uint8_t crntSubpass = 0;

    //std::vector<Metal_FramebufferSubpass> subpasses;
    std::vector<void* /*MTLRenderPassDescriptor*/> renderPasses;

    std::vector<Metal_FramebufferAttachment> colorAttachments;
    Metal_FramebufferAttachment depthAttachment;
    //std::vector<Metal_FramebufferAttachment*> sortedAttachments;

public:
    Metal_Framebuffer(Metal_FramebufferCreateInfo createInfo);

    ~Metal_Framebuffer();

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline void* /*MTLRenderPassDescriptor*/ renderPass(uint8_t index) { return renderPasses[index]; }
};

} //namespace lv

#endif
