#ifndef LV_DENOISER_H
#define LV_DENOISER_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/denoiser.hpp"

namespace lv {

typedef vulkan::DenoiserCreateInfo DenoiserCreateInfo;
typedef vulkan::Denoiser Denoiser;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/denoiser.hpp"

namespace lv {

typedef metal::DenoiserCreateInfo DenoiserCreateInfo;
typedef metal::Denoiser Denoiser;

} //namespace lv

#endif

#endif
