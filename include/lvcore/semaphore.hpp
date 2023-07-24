#ifndef LV_SEMAPHORE_H
#define LV_SEMAPHORE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/semaphore.hpp"

namespace lv {

typedef vulkan::Semaphore Semaphore;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/semaphore.hpp"

namespace lv {

typedef metal::Semaphore Semaphore;

} //namespace lv

#endif

#endif
