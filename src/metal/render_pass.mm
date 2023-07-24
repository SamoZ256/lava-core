#include "metal/lvcore/render_pass.hpp"

#include <iostream>

namespace lv {

namespace metal {

RenderPass::RenderPass(internal::RenderPassCreateInfo createInfo) {
    subpasses = createInfo.subpasses;
    attachments = createInfo.attachments;
    sortedAttachments.resize(attachments.size());
    for (uint8_t i = 0; i < attachments.size(); i++)
        sortedAttachments[attachments[i].index] = &attachments[i];
}

} //namespace metal

} //namespace lv
