#ifndef LV_METAL_VERTEX_DESCRIPTOR_H
#define LV_METAL_VERTEX_DESCRIPTOR_H

#include "lvcore/internal/vertex_descriptor.hpp"

#include "common.hpp"

namespace lv {

namespace metal {

class VertexDescriptor : public internal::VertexDescriptor {
private:
    void* /*MTLVertexDescriptor*/ _descriptor;

public:
    VertexDescriptor(internal::VertexDescriptorCreateInfo createInfo);

    //Getters
    inline void* /*MTLVertexDescriptor*/ descriptor() { return _descriptor; }
};

} //namespace metal

} //namespace lv

#endif
