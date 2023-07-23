#ifndef LV_VULKAN_ATTACHMENT_H
#define LV_VULKAN_ATTACHMENT_H

#include "lvcore/internal/common.hpp"

#include "vulkan/lvcore/core/core.hpp"

#include "image.hpp"

namespace lv {

struct Vulkan_ColorBlendAttachment {
    uint8_t index = 0;
    Bool blendEnable = False;
    BlendFactor srcRgbBlendFactor = BlendFactor::SourceAlpha;
    BlendFactor dstRgbBlendFactor = BlendFactor::OneMinusSourceAlpha;
    BlendOperation rgbBlendOp = BlendOperation::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::One;
    BlendOperation alphaBlendOp = BlendOperation::Max;
};

struct Vulkan_RenderPassAttachment {
    uint8_t index = 0;
    Format format;
    AttachmentLoadOperation loadOp = AttachmentLoadOperation::DontCare;
    AttachmentStoreOperation storeOp = AttachmentStoreOperation::DontCare;
    ImageLayout initialLayout = ImageLayout::Undefined;
    ImageLayout finalLayout = ImageLayout::Undefined;

    VkAttachmentDescription getAttachmentDescription(/*VkImageLayout finalLayout*/) {
        VkFormat vkFormat;
        GET_VK_FORMAT(format, vkFormat);
        VkAttachmentLoadOp vkAttachmentLoadOp;
        GET_VK_ATTACHMENT_LOAD_OP(loadOp, vkAttachmentLoadOp);
        VkAttachmentStoreOp vkAttachmentStoreOp;
        GET_VK_ATTACHMENT_STORE_OP(storeOp, vkAttachmentStoreOp);
        VkImageLayout vkInitialImageLayout, vkFinalImageLayout;
        GET_VK_IMAGE_LAYOUT(initialLayout, vkInitialImageLayout);
        GET_VK_IMAGE_LAYOUT(finalLayout, vkFinalImageLayout);

        VkAttachmentDescription description{};
        description.format = vkFormat;
        description.samples = VK_SAMPLE_COUNT_1_BIT;
        description.loadOp = vkAttachmentLoadOp;
        description.storeOp = vkAttachmentStoreOp;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.initialLayout = vkInitialImageLayout;
        description.finalLayout = vkFinalImageLayout;

        return description;
    }
};

struct Vulkan_SubpassAttachment {
    int8_t index = -1;
    ImageLayout layout = ImageLayout::Undefined;

    VkAttachmentReference getAttachmentReference() {
        VkImageLayout vkImageLayout;
        GET_VK_IMAGE_LAYOUT(layout, vkImageLayout);

        VkAttachmentReference reference{};
        reference.attachment = index;
        reference.layout = vkImageLayout;

        return reference;
    }
};

struct Vulkan_FramebufferAttachment {
    int8_t index = -1;
    Vulkan_Image* image = nullptr;
    ClearValue clearValue{};
};

} //namespace lv

#endif
