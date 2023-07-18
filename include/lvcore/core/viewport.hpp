#ifndef LV_VIEWPORT_H
#define LV_VIEWPORT_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/viewport.hpp"

namespace lv {

typedef Vulkan_Viewport Viewport;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/viewport.hpp"

namespace lv {

typedef Metal_Viewport Viewport;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/viewport.hpp"

namespace lv {

typedef OpenGL_Viewport Viewport;

} //namespace lv

#endif

#endif
