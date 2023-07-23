#ifndef LV_BUFFER_H
#define LV_BUFFER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/buffer.hpp"

namespace lv {

typedef Vulkan_BufferCreateInfo BufferCreateInfo;
typedef Vulkan_Buffer Buffer;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/buffer.hpp"

namespace lv {

typedef Metal_BufferCreateInfo BufferCreateInfo;
typedef Metal_Buffer Buffer;

} //namespace lv

#endif

#endif
