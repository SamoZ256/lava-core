#ifndef LV_VERTEX_DESCRIPTOR_H
#define LV_VERTEX_DESCRIPTOR_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/vertex_descriptor.hpp"

namespace lv {

typedef vulkan::VertexDescriptor VertexDescriptor;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/vertex_descriptor.hpp"

namespace lv {

typedef metal::VertexDescriptor VertexDescriptor;

} //namespace lv

#endif

namespace lv {

typedef internal::VertexDescriptorBinding VertexDescriptorBinding;
typedef internal::VertexDescriptorCreateInfo VertexDescriptorCreateInfo;

} //namespace lv

#endif
