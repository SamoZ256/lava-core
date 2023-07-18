#ifndef LV_VULKAN_ATTACHMENT_H
#define LV_VULKAN_ATTACHMENT_H

#include "image.hpp"

namespace lv {

struct Vulkan_ColorBlendAttachment {
    uint8_t index = 0;
    bool blendEnable = LV_FALSE;
    LvBlendFactor srcRgbBlendFactor = LV_BLEND_FACTOR_SRC_ALPHA;
    LvBlendFactor dstRgbBlendFactor = LV_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    LvBlendOp rgbBlendOp = LV_BLEND_OP_ADD;
    LvBlendFactor srcAlphaBlendFactor = LV_BLEND_FACTOR_ONE;
    LvBlendFactor dstAlphaBlendFactor = LV_BLEND_FACTOR_ONE;
    LvBlendOp alphaBlendOp = LV_BLEND_OP_MAX;
};

struct Vulkan_RenderPassAttachment {
    uint8_t index = 0;
    LvFormat format;
    LvAttachmentLoadOp loadOp = LV_ATTACHMENT_LOAD_OP_DONT_CARE;
    LvAttachmentStoreOp storeOp = LV_ATTACHMENT_STORE_OP_DONT_CARE;
    LvImageLayout initialLayout = LV_IMAGE_LAYOUT_UNDEFINED;
    LvImageLayout finalLayout = LV_IMAGE_LAYOUT_UNDEFINED;

    VkAttachmentDescription getAttachmentDescription(/*VkImageLayout finalLayout*/) {
        VkAttachmentDescription description{};
        description.format = format;
        description.samples = VK_SAMPLE_COUNT_1_BIT;
        description.loadOp = loadOp;
        description.storeOp = storeOp;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.initialLayout = initialLayout;
        description.finalLayout = finalLayout;

        return description;
    }
};

struct Vulkan_SubpassAttachment {
    int8_t index = -1;
    LvImageLayout layout = LV_IMAGE_LAYOUT_UNDEFINED;

    VkAttachmentReference getAttachmentReference() {
        VkAttachmentReference reference{};
        reference.attachment = index;
        reference.layout = layout;

        return reference;
    }
};

struct Vulkan_FramebufferAttachment {
    int8_t index = -1;
    Vulkan_Image* image = nullptr;
    LvClearValue clearValue = {
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
        .depthStencil = {1.0f, 0}
    };
};

} //namespace lv

#endif
