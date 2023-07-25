#ifndef LV_IMAGE_H
#define LV_IMAGE_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/image.hpp"

namespace lv {

typedef vulkan::Image Image;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/image.hpp"

namespace lv {

typedef metal::Image Image;

} //namespace lv

#endif

namespace lv {

typedef internal::ImageCreateInfo ImageCreateInfo;
typedef internal::ImageLoadInfo ImageLoadInfo;
typedef internal::ImageViewCreateInfo ImageViewCreateInfo;

} //namespace lv

#endif
