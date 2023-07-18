#ifndef LV_VULKAN_RENDER_PASS_H
#define LV_VULKAN_RENDER_PASS_H

#include <vector>

#include "attachment.hpp"

namespace lv {

struct Vulkan_SubpassCreateInfo {
    std::vector<Vulkan_SubpassAttachment> colorAttachments;
    Vulkan_SubpassAttachment depthAttachment;
    std::vector<Vulkan_SubpassAttachment> inputAttachments;
};

class Vulkan_Subpass {
public:
    std::vector<Vulkan_SubpassAttachment> colorAttachments;
    Vulkan_SubpassAttachment depthAttachment;
    std::vector<Vulkan_SubpassAttachment> inputAttachments;

    Vulkan_Subpass(Vulkan_SubpassCreateInfo createInfo) : colorAttachments(createInfo.colorAttachments), depthAttachment(createInfo.depthAttachment), inputAttachments(createInfo.inputAttachments) {}
};

struct Vulkan_RenderPassCreateInfo {
    std::vector<Vulkan_Subpass*> subpasses;
    std::vector<Vulkan_RenderPassAttachment> attachments;
    std::vector<VkSubpassDependency> dependencies;
};

class Vulkan_RenderPass {
private:
    VkRenderPass _renderPass;

    std::vector<VkSubpassDependency> dependencies;

public:
    Vulkan_RenderPass(Vulkan_RenderPassCreateInfo createInfo);

    ~Vulkan_RenderPass();

    //Getters
    inline VkRenderPass renderPass() { return _renderPass; }
};

} //namespace lv

#endif
