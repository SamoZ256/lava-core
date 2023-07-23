#ifndef LV_SAMPLER_H
#define LV_SAMPLER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/sampler.hpp"

namespace lv {

typedef Vulkan_SamplerCreateInfo SamplerCreateInfo;
typedef Vulkan_Sampler Sampler;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/sampler.hpp"

namespace lv {

typedef Metal_SamplerCreateInfo SamplerCreateInfo;
typedef Metal_Sampler Sampler;

} //namespace lv

#endif

#endif
