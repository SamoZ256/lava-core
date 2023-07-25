#ifndef LV_INSTANCE_H
#define LV_INSTANCE_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/instance.hpp"

#define g_instance g_vulkan_instance

namespace lv {

typedef vulkan::Instance Instance;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/instance.hpp"

#define g_instance g_metal_instance

namespace lv {

typedef metal::Instance Instance;

} //namespace lv

#endif

namespace lv {

typedef internal::InstanceCreateInfo InstanceCreateInfo;

} //namespace lv

#endif
