#ifndef LV_BUFFER_H
#define LV_BUFFER_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/buffer.hpp"

namespace lv {

typedef vulkan::Buffer Buffer;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/buffer.hpp"

namespace lv {

typedef metal::Buffer Buffer;

} //namespace lv

#endif

namespace lv {

typedef internal::BufferCreateInfo BufferCreateInfo;

} //namespace lv

#endif
