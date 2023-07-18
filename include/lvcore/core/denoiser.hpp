#ifndef LV_DENOISER_H
#define LV_DENOISER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/denoiser.hpp"

namespace lv {

typedef Vulkan_DenoiserCreateInfo DenoiserCreateInfo;
typedef Vulkan_Denoiser Denoiser;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/denoiser.hpp"

namespace lv {

typedef Metal_DenoiserCreateInfo DenoiserCreateInfo;
typedef Metal_Denoiser Denoiser;

} //namespace lv

#endif

#endif
