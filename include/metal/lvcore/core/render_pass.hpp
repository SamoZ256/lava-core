#ifndef LV_METAL_RENDER_PASS_H
#define LV_METAL_RENDER_PASS_H

#include "attachment.hpp"

namespace lv {

struct Metal_SubpassCreateInfo {
    std::vector<Metal_SubpassAttachment> colorAttachments;
    Metal_SubpassAttachment depthAttachment;
    std::vector<Metal_SubpassAttachment> inputAttachments;
};

class Metal_Subpass {
public:
    std::vector<Metal_SubpassAttachment> colorAttachments;
    Metal_SubpassAttachment depthAttachment;
    std::vector<Metal_SubpassAttachment> inputAttachments;

    Metal_Subpass(Metal_SubpassCreateInfo createInfo) : colorAttachments(createInfo.colorAttachments), depthAttachment(createInfo.depthAttachment), inputAttachments(createInfo.inputAttachments) {}
};

struct Metal_RenderPassCreateInfo {
    std::vector<Metal_Subpass*> subpasses;
    std::vector<Metal_RenderPassAttachment> attachments;
};

class Metal_RenderPass {
public:
    std::vector<Metal_Subpass*> subpasses;
    std::vector<Metal_RenderPassAttachment> attachments;
    std::vector<Metal_RenderPassAttachment*> sortedAttachments;

    Metal_RenderPass(Metal_RenderPassCreateInfo createInfo);

    ~Metal_RenderPass() {}
};

} //namespace lv

#endif
