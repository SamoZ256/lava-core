#ifndef LV_METAL_VERTEX_DESCRIPTOR_H
#define LV_METAL_VERTEX_DESCRIPTOR_H

#include "common.hpp"

#include "enums.hpp"

namespace lv {

struct Metal_VertexDescriptorBinding {
    uint16_t location;
    LvVertexFormat format;
    uint32_t offset;
};

struct Metal_VertexDescriptorCreateInfo {
    size_t size;
    LvVertexStepFunction stepFunction = LV_VERTEX_STEP_FUNCTION_PER_VERTEX;
    std::vector<Metal_VertexDescriptorBinding> bindings;
};

class Metal_VertexDescriptor {
private:
    void* /*MTLVertexDescriptor*/ _descriptor;

public:
    Metal_VertexDescriptor(Metal_VertexDescriptorCreateInfo createInfo);

    //Getters
    inline void* /*MTLVertexDescriptor*/ descriptor() { return _descriptor; }
};

} //namespace lv

#endif
