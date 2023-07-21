#ifndef LV_METAL_ATTACHMENT_H
#define LV_METAL_ATTACHMENT_H

#include "lvcore/core/common.hpp"

#include "image.hpp"

namespace lv {

struct Metal_ColorBlendAttachment {
    uint8_t index = 0;
    Bool blendEnable = False;
    BlendFactor srcRgbBlendFactor = BlendFactor::SourceAlpha;
    BlendFactor dstRgbBlendFactor = BlendFactor::OneMinusSourceAlpha;
    BlendOperation rgbBlendOp = BlendOperation::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::One;
    BlendOperation alphaBlendOp = BlendOperation::Max;
};

struct Metal_RenderPassAttachment {
    uint8_t index = 0;
    lv::Format format;
    lv::AttachmentLoadOperation loadOp = AttachmentLoadOperation::DontCare;
    lv::AttachmentStoreOperation storeOp = AttachmentStoreOperation::DontCare;
    ImageLayout initialLayout = ImageLayout::Undefined;
    ImageLayout finalLayout = ImageLayout::Undefined;
};

struct Metal_SubpassAttachment {
    int8_t index = -1;
    ImageLayout layout = ImageLayout::Undefined;
};

struct Metal_FramebufferAttachment {
    int8_t index = -1;
    Metal_Image* image = nullptr;
    ClearValue clearValue{};
};

} //namespace lv

#endif
