#ifndef LV_INTERNAL_RENDER_PASS_H
#define LV_INTERNAL_RENDER_PASS_H

#include "attachment.hpp"

namespace lv {

namespace internal {

struct SubpassCreateInfo {
    std::vector<SubpassAttachment> colorAttachments;
    SubpassAttachment depthAttachment;
    std::vector<SubpassAttachment> inputAttachments;
};

class Subpass {

};

struct RenderPassCreateInfo {
    std::vector<Subpass*> subpasses;
    std::vector<RenderPassAttachment> attachments;
};

class RenderPass {
public:
    virtual ~RenderPass() {}
};

} //namespace internal

} //namespace lv

#endif
