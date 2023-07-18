#ifndef LV_INSTANCE_H
#define LV_INSTANCE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/instance.hpp"

#define g_instance g_vulkan_instance

namespace lv {

typedef Vulkan_InstanceCreateInfo InstanceCreateInfo;
typedef Vulkan_Instance Instance;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/instance.hpp"

#define g_instance g_metal_instance

namespace lv {

typedef Metal_InstanceCreateInfo InstanceCreateInfo;
typedef Metal_Instance Instance;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/instance.hpp"

#define g_instance g_opengl_instance

namespace lv {

typedef OpenGL_InstanceCreateInfo InstanceCreateInfo;
typedef OpenGL_Instance Instance;

} //namespace lv

#endif

#endif
