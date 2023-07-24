#ifndef LV_RENDER_PASS_H
#define LV_RENDER_PASS_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/render_pass.hpp"

namespace lv {

typedef vulkan::Subpass Subpass;
typedef vulkan::RenderPass RenderPass;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/render_pass.hpp"

namespace lv {

typedef metal::Subpass Subpass;
typedef metal::RenderPass RenderPass;

} //namespace lv

#endif

#endif
