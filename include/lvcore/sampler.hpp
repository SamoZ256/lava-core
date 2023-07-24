#ifndef LV_SAMPLER_H
#define LV_SAMPLER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/sampler.hpp"

namespace lv {

typedef vulkan::Sampler Sampler;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/sampler.hpp"

namespace lv {

typedef metal::Sampler Sampler;

} //namespace lv

#endif

namespace lv {

typedef internal::SamplerCreateInfo SamplerCreateInfo;

} //namespace lv

#endif
