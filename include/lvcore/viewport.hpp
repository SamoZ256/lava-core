#ifndef LV_VIEWPORT_H
#define LV_VIEWPORT_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/viewport.hpp"

namespace lv {

typedef vulkan::Viewport Viewport;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/viewport.hpp"

namespace lv {

typedef metal::Viewport Viewport;

} //namespace lv

#endif

#endif
