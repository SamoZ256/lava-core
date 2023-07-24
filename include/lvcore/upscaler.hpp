#ifndef LV_UPSCALER_H
#define LV_UPSCALER_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/upscaler.hpp"

namespace lv {

typedef vulkan::UpscalerCreateInfo UpscalerCreateInfo;
typedef vulkan::Upscaler Upscaler;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/upscaler.hpp"

namespace lv {

typedef metal::UpscalerCreateInfo UpscalerCreateInfo;
typedef metal::Upscaler Upscaler;

} //namespace lv

#endif

#endif
