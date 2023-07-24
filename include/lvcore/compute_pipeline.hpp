#ifndef LV_COMPUTE_PIPELINE_H
#define LV_COMPUTE_PIPELINE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/compute_pipeline.hpp"

namespace lv {
typedef vulkan::ComputePipeline ComputePipeline;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/compute_pipeline.hpp"

namespace lv {
typedef metal::ComputePipeline ComputePipeline;

} //namespace lv

#endif

namespace lv {

typedef internal::ComputePipelineCreateInfo ComputePipelineCreateInfo;

} //namespace lv

#endif
