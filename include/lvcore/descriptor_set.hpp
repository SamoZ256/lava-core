#ifndef LV_DESCRIPTOR_SET_H
#define LV_DESCRIPTOR_SET_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/descriptor_set.hpp"

namespace lv {
typedef vulkan::DescriptorSet DescriptorSet;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/descriptor_set.hpp"

namespace lv {
typedef metal::DescriptorSet DescriptorSet;

} //namespace lv

#endif

namespace lv {

typedef internal::DescriptorSetCreateInfo DescriptorSetCeateInfo;

} //namespace lv

#endif
