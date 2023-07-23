#ifndef LV_DEVICE_H
#define LV_DEVICE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/device.hpp"

#define g_device g_vulkan_device

namespace lv {

typedef Vulkan_DeviceCreateInfo DeviceCreateInfo;
typedef Vulkan_Device Device;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/device.hpp"

#define g_device g_metal_device

namespace lv {

typedef Metal_DeviceCreateInfo DeviceCreateInfo;
typedef Metal_Device Device;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/device.hpp"

#define g_device g_opengl_device

namespace lv {

typedef OpenGL_DeviceCreateInfo DeviceCreateInfo;
typedef OpenGL_Device Device;

} //namespace lv

#endif

#endif
