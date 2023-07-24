#ifndef LV_VULKAN_RENDER_PASS_H
#define LV_VULKAN_RENDER_PASS_H

#include <vector>

#include "attachment.hpp"

namespace lv {

namespace vulkan {

struct SubpassCreateInfo {
    std::vector<SubpassAttachment> colorAttachments;
    SubpassAttachment depthAttachment;
    std::vector<SubpassAttachment> inputAttachments;
};

class Subpass {
public:
    std::vector<SubpassAttachment> colorAttachments;
    SubpassAttachment depthAttachment;
    std::vector<SubpassAttachment> inputAttachments;

    Subpass(SubpassCreateInfo createInfo) : colorAttachments(createInfo.colorAttachments), depthAttachment(createInfo.depthAttachment), inputAttachments(createInfo.inputAttachments) {}
};

struct RenderPassCreateInfo {
    std::vector<Subpass*> subpasses;
    std::vector<RenderPassAttachment> attachments;
    std::vector<VkSubpassDependency> dependencies;
};

class RenderPass {
private:
    VkRenderPass _renderPass;

    std::vector<VkSubpassDependency> dependencies;

public:
    RenderPass(RenderPassCreateInfo createInfo);

    ~RenderPass();

    //Getters
    inline VkRenderPass renderPass() { return _renderPass; }
};

} //namespace vulkan

} //namespace lv

#endif
