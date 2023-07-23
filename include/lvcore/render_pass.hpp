#ifndef LV_RENDER_PASS_H
#define LV_RENDER_PASS_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/render_pass.hpp"

namespace lv {

typedef Vulkan_Subpass Subpass;
typedef Vulkan_RenderPass RenderPass;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/render_pass.hpp"

namespace lv {

typedef Metal_Subpass Subpass;
typedef Metal_RenderPass RenderPass;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/render_pass.hpp"

namespace lv {

typedef OpenGL_Subpass Subpass;
typedef OpenGL_RenderPass RenderPass;

} //namespace lv

#endif

#endif
