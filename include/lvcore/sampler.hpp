#ifndef LV_SAMPLER_H
#define LV_SAMPLER_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/sampler.hpp"

namespace lv {

typedef vulkan::Sampler Sampler;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/sampler.hpp"

namespace lv {

typedef metal::Sampler Sampler;

} //namespace lv

#endif

namespace lv {

typedef internal::SamplerCreateInfo SamplerCreateInfo;

} //namespace lv

#endif
