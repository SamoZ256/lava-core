#ifndef LV_GRAPHICS_PIPELINE_H
#define LV_GRAPHICS_PIPELINE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/graphics_pipeline.hpp"

namespace lv {

typedef vulkan::GraphicsPipeline GraphicsPipeline;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/graphics_pipeline.hpp"

namespace lv {

typedef metal::GraphicsPipeline GraphicsPipeline;

} //namespace lv

#endif

namespace lv {

typedef internal::GraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;

} //namespace lv

#endif
