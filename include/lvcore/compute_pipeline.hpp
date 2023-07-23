#ifndef LV_COMPUTE_PIPELINE_H
#define LV_COMPUTE_PIPELINE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/compute_pipeline.hpp"

namespace lv {

typedef Vulkan_ComputePipelineCreateInfo ComputePipelineCreateInfo;
typedef Vulkan_ComputePipeline ComputePipeline;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/compute_pipeline.hpp"

namespace lv {

typedef Metal_ComputePipelineCreateInfo ComputePipelineCreateInfo;
typedef Metal_ComputePipeline ComputePipeline;

} //namespace lv

#endif

#endif
