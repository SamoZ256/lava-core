#ifndef LV_PIPELINE_LAYOUT_H
#define LV_PIPELINE_LAYOUT_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/pipeline_layout.hpp"

namespace lv {

typedef vulkan::DescriptorSetLayout DescriptorSetLayout;
typedef vulkan::PipelineLayout PipelineLayout;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/pipeline_layout.hpp"

namespace lv {

typedef metal::DescriptorSetLayout DescriptorSetLayout;
typedef metal::PipelineLayout PipelineLayout;

} //namespace lv

#endif

#endif
