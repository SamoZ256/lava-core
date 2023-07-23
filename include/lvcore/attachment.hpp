#ifndef LV_ATTACHMENT_H
#define LV_ATTACHMENT_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/attachment.hpp"

namespace lv {

typedef Vulkan_ColorBlendAttachment ColorBlendAttachment;
typedef Vulkan_RenderPassAttachment RenderPassAttachment;
typedef Vulkan_SubpassAttachment SubpassAttachment;
typedef Vulkan_FramebufferAttachment FramebufferAttachment;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/attachment.hpp"

namespace lv {

typedef Metal_ColorBlendAttachment ColorBlendAttachment;
typedef Metal_RenderPassAttachment RenderPassAttachment;
typedef Metal_SubpassAttachment SubpassAttachment;
typedef Metal_FramebufferAttachment FramebufferAttachment;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/attachment.hpp"

namespace lv {

typedef Vulkan_ColorBlendAttachment ColorBlendAttachment;
typedef OpenGL_RenderPassAttachment RenderPassAttachment;
typedef OpenGL_SubpassAttachment SubpassAttachment;
typedef OpenGL_FramebufferAttachment FramebufferAttachment;

} //namespace lv

#endif

#endif
