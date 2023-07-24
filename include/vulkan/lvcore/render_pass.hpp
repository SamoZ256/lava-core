#ifndef LV_VULKAN_RENDER_PASS_H
#define LV_VULKAN_RENDER_PASS_H

#include <vulkan/vulkan.h>

#include "lvcore/internal/render_pass.hpp"

namespace lv {

namespace vulkan {

class Subpass : public internal::Subpass {
public:
    std::vector<internal::SubpassAttachment> colorAttachments;
    internal::SubpassAttachment depthAttachment;
    std::vector<internal::SubpassAttachment> inputAttachments;

    Subpass(internal::SubpassCreateInfo createInfo) : colorAttachments(createInfo.colorAttachments), depthAttachment(createInfo.depthAttachment), inputAttachments(createInfo.inputAttachments) {}
};

class RenderPass : public internal::RenderPass {
private:
    VkRenderPass _renderPass;

    std::vector<VkSubpassDependency> dependencies;

public:
    RenderPass(internal::RenderPassCreateInfo createInfo, std::vector<VkSubpassDependency> aDependencies = {});

    ~RenderPass() override;

    //Getters
    inline VkRenderPass renderPass() { return _renderPass; }
};

} //namespace vulkan

} //namespace lv

#endif
