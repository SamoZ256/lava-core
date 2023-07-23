#ifndef LV_SEMAPHORE_H
#define LV_SEMAPHORE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/semaphore.hpp"

namespace lv {

typedef Vulkan_Semaphore Semaphore;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/semaphore.hpp"

namespace lv {

typedef Metal_Semaphore Semaphore;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/semaphore.hpp"

namespace lv {

typedef OpenGL_Semaphore Semaphore;

} //namespace lv

#endif

#endif
