#ifndef LV_GRAPHICS_PIPELINE_H
#define LV_GRAPHICS_PIPELINE_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/graphics_pipeline.hpp"

namespace lv {

typedef Vulkan_GraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
typedef Vulkan_GraphicsPipeline GraphicsPipeline;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/graphics_pipeline.hpp"

namespace lv {

typedef Metal_GraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
typedef Metal_GraphicsPipeline GraphicsPipeline;

} //namespace lv

#endif

#endif
