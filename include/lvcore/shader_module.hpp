#ifndef LV_SHADER_MODULE_H
#define LV_SHADER_MODULE_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/shader_module.hpp"

namespace lv {

typedef vulkan::ShaderModule ShaderModule;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/shader_module.hpp"

namespace lv {

typedef metal::ShaderModule ShaderModule;

} //namespace lv

#endif

namespace lv {

typedef internal::ShaderModuleCreateInfo ShaderModuleCreateInfo;

} //namespace lv

#endif
