#ifndef LV_ATTACHMENT_H
#define LV_ATTACHMENT_H

#include "internal/attachment.hpp"

namespace lv {

typedef internal::ColorBlendAttachment ColorBlendAttachment;
typedef internal::RenderPassAttachment RenderPassAttachment;
typedef internal::SubpassAttachment SubpassAttachment;
typedef internal::FramebufferAttachment FramebufferAttachment;

} //namespace lv

#endif
