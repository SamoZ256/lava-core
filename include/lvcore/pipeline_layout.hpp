#ifndef LV_PIPELINE_LAYOUT_H
#define LV_PIPELINE_LAYOUT_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/pipeline_layout.hpp"

namespace lv {

typedef Vulkan_DescriptorSetLayout DescriptorSetLayout;
typedef Vulkan_PipelineLayout PipelineLayout;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/pipeline_layout.hpp"

namespace lv {

typedef Metal_DescriptorSetLayout DescriptorSetLayout;
typedef Metal_PipelineLayout PipelineLayout;

} //namespace lv

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/pipeline_layout.hpp"

namespace lv {

typedef OpenGL_DescriptorSetLayout DescriptorSetLayout;
typedef OpenGL_PipelineLayout PipelineLayout;

} //namespace lv

#endif

#endif
