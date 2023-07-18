#ifndef LV_UPSCALER_H
#define LV_UPSCALER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/upscaler.hpp"

namespace lv {

typedef Vulkan_UpscalerCreateInfo UpscalerCreateInfo;
typedef Vulkan_Upscaler Upscaler;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/upscaler.hpp"

namespace lv {

typedef Metal_UpscalerCreateInfo UpscalerCreateInfo;
typedef Metal_Upscaler Upscaler;

} //namespace lv

#endif

#endif
