#ifndef LV_SEMAPHORE_H
#define LV_SEMAPHORE_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/semaphore.hpp"

namespace lv {

typedef vulkan::Semaphore Semaphore;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/semaphore.hpp"

namespace lv {

typedef metal::Semaphore Semaphore;

} //namespace lv

#endif

#endif
