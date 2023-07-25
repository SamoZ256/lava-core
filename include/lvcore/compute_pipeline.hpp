#ifndef LV_COMPUTE_PIPELINE_H
#define LV_COMPUTE_PIPELINE_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/compute_pipeline.hpp"

namespace lv {
typedef vulkan::ComputePipeline ComputePipeline;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/compute_pipeline.hpp"

namespace lv {
typedef metal::ComputePipeline ComputePipeline;

} //namespace lv

#endif

namespace lv {

typedef internal::ComputePipelineCreateInfo ComputePipelineCreateInfo;

} //namespace lv

#endif
