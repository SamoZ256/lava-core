#ifndef LV_DEVICE_H
#define LV_DEVICE_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/device.hpp"

#define g_device g_vulkan_device

namespace lv {

typedef vulkan::Device Device;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/device.hpp"

#define g_device g_metal_device

namespace lv {
typedef metal::Device Device;

} //namespace lv

#endif

namespace lv {

typedef internal::DeviceCreateInfo DeviceCreateInfo;

} //namespace lv

#endif
