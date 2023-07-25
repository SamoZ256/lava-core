#ifndef LV_METAL_RENDER_PASS_H
#define LV_METAL_RENDER_PASS_H

#include "lvcore/internal/render_pass.hpp"

namespace lv {

namespace metal {

class Subpass : public internal::Subpass {
public:
    std::vector<internal::SubpassAttachment> colorAttachments;
    internal::SubpassAttachment depthAttachment;
    std::vector<internal::SubpassAttachment> inputAttachments;

    Subpass(internal::SubpassCreateInfo createInfo) : colorAttachments(createInfo.colorAttachments), depthAttachment(createInfo.depthAttachment), inputAttachments(createInfo.inputAttachments) {}
};

class RenderPass : public internal::RenderPass {
public:
    std::vector<internal::Subpass*> subpasses;
    std::vector<internal::RenderPassAttachment> attachments;
    std::vector<internal::RenderPassAttachment*> sortedAttachments;

    RenderPass(internal::RenderPassCreateInfo createInfo);

    ~RenderPass() override {}
};

} //namespace metal

} //namespace lv

#endif
