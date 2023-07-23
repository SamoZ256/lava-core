#ifndef LV_SHADER_MODULE_H
#define LV_SHADER_MODULE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/shader_module.hpp"

namespace lv {

typedef Vulkan_ShaderModuleCreateInfo ShaderModuleCreateInfo;
typedef Vulkan_ShaderModule ShaderModule;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/shader_module.hpp"

namespace lv {

typedef Metal_ShaderModuleCreateInfo ShaderModuleCreateInfo;
typedef Metal_ShaderModule ShaderModule;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/shader_module.hpp"

namespace lv {

typedef OpenGL_ShaderModuleCreateInfo ShaderModuleCreateInfo;
typedef OpenGL_ShaderModule ShaderModule;

} //namespace lv

#endif

#endif
