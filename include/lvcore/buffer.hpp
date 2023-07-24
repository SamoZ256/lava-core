#ifndef LV_BUFFER_H
#define LV_BUFFER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/buffer.hpp"

namespace lv {

typedef vulkan::Buffer Buffer;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/buffer.hpp"

namespace lv {

typedef metal::Buffer Buffer;

} //namespace lv

#endif

namespace lv {

typedef internal::BufferCreateInfo BufferCreateInfo;

} //namespace lv

#endif
