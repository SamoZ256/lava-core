#ifndef LV_DESCRIPTOR_SET_H
#define LV_DESCRIPTOR_SET_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/descriptor_set.hpp"

namespace lv {

typedef Vulkan_DescriptorSetCreateInfo DescriptorSetCeateInfo;
typedef Vulkan_DescriptorSet DescriptorSet;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/descriptor_set.hpp"

namespace lv {

typedef Metal_DescriptorSetCreateInfo DescriptorSetCeateInfo;
typedef Metal_DescriptorSet DescriptorSet;

} //namespace lv

#endif

#endif
