#ifndef LV_RENDER_PASS_H
#define LV_RENDER_PASS_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/render_pass.hpp"

namespace lv {

typedef vulkan::Subpass Subpass;
typedef vulkan::RenderPass RenderPass;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/render_pass.hpp"

namespace lv {

typedef metal::Subpass Subpass;
typedef metal::RenderPass RenderPass;

} //namespace lv

#endif

#endif
