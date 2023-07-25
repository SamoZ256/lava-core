#ifndef LV_FRAMEBUFFER_H
#define LV_FRAMEBUFFER_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/framebuffer.hpp"

namespace lv {

typedef vulkan::Framebuffer Framebuffer;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/framebuffer.hpp"

namespace lv {

typedef metal::Framebuffer Framebuffer;

} //namespace lv

#endif

#endif
