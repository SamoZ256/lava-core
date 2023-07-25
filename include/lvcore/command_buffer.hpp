#ifndef LV_COMMAND_BUFFER_H
#define LV_COMMAND_BUFFER_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/command_buffer.hpp"

namespace lv {

typedef vulkan::CommandBuffer CommandBuffer;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/command_buffer.hpp"

namespace lv {
typedef metal::CommandBuffer CommandBuffer;

} //namespace lv

#endif

namespace lv {

typedef internal::CommandBufferCreateInfo CommandBufferCreateInfo;

} //namespace lv

#endif
