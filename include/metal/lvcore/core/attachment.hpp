#ifndef LV_METAL_ATTACHMENT_H
#define LV_METAL_ATTACHMENT_H

#include "enums.hpp"

#include "image.hpp"

namespace lv {

struct Metal_ColorBlendAttachment {
    uint8_t index = 0;
    bool blendEnable = LV_FALSE;
    LvBlendFactor srcRgbBlendFactor = LV_BLEND_FACTOR_SRC_ALPHA;
    LvBlendFactor dstRgbBlendFactor = LV_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    LvBlendOp rgbBlendOp = LV_BLEND_OP_ADD;
    LvBlendFactor srcAlphaBlendFactor = LV_BLEND_FACTOR_ONE;
    LvBlendFactor dstAlphaBlendFactor = LV_BLEND_FACTOR_ONE;
    LvBlendOp alphaBlendOp = LV_BLEND_OP_MAX;
};

struct Metal_RenderPassAttachment {
    uint8_t index = 0;
    lv::Format format;
    LvAttachmentLoadOp loadOp = LV_ATTACHMENT_LOAD_OP_DONT_CARE;
    LvAttachmentStoreOp storeOp = LV_ATTACHMENT_STORE_OP_DONT_CARE;
    LvImageLayout initialLayout = LV_IMAGE_LAYOUT_UNDEFINED;
    LvImageLayout finalLayout = LV_IMAGE_LAYOUT_UNDEFINED;
};

struct Metal_SubpassAttachment {
    int8_t index = -1;
    LvImageLayout layout = LV_IMAGE_LAYOUT_UNDEFINED;
};

struct Metal_FramebufferAttachment {
    int8_t index = -1;
    Metal_Image* image = nullptr;
    LvClearValue clearValue{};
};

} //namespace lv

#endif
