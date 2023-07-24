#ifndef LV_INTERNAL_VERTEX_DESCRIPTOR_H
#define LV_INTERNAL_VERTEX_DESCRIPTOR_H

#include "common.hpp"

namespace lv {

namespace internal {

struct VertexDescriptorBinding {
    uint16_t location;
    Format format;
    uint32_t offset;
};

struct VertexDescriptorCreateInfo {
    size_t size;
    VertexInputRate inputRate = VertexInputRate::PerVertex;
    std::vector<VertexDescriptorBinding> bindings;
};

class VertexDescriptor {
    
};

} //namespace internal

} //namespace lv

#endif
