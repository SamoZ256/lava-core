#ifndef LV_VULKAN_ATTACHMENT_H
#define LV_VULKAN_ATTACHMENT_H

#include "lvcore/internal/common.hpp"

#include "vulkan/lvcore/core.hpp"

#include "image.hpp"

namespace lv {

namespace vulkan {

struct ColorBlendAttachment {
    uint8_t index = 0;
    Bool blendEnable = False;
    BlendFactor srcRgbBlendFactor = BlendFactor::SourceAlpha;
    BlendFactor dstRgbBlendFactor = BlendFactor::OneMinusSourceAlpha;
    BlendOperation rgbBlendOp = BlendOperation::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::One;
    BlendOperation alphaBlendOp = BlendOperation::Max;
};

struct RenderPassAttachment {
    uint8_t index = 0;
    Format format;
    AttachmentLoadOperation loadOp = AttachmentLoadOperation::DontCare;
    AttachmentStoreOperation storeOp = AttachmentStoreOperation::DontCare;
    ImageLayout initialLayout = ImageLayout::Undefined;
    ImageLayout finalLayout = ImageLayout::Undefined;
};

struct SubpassAttachment {
    int8_t index = -1;
    ImageLayout layout = ImageLayout::Undefined;
};

struct FramebufferAttachment {
    int8_t index = -1;
    Image* image = nullptr;
    ClearValue clearValue{};
};

} //namespace vulkan

} //namespace lv

#endif
