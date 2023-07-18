#include "metal/lvcore/core/vertex_descriptor.hpp"

#include "metal/lvcore/core/common.hpp"

namespace lv {

Metal_VertexDescriptor::Metal_VertexDescriptor(Metal_VertexDescriptorCreateInfo createInfo) {
    _descriptor = [[MTLVertexDescriptor alloc] init];
    ((MTLVertexDescriptor*)_descriptor).layouts[LV_METAL_VERTEX_BUFFER_BINDING_INDEX].stride = createInfo.size;
    //TODO: check if this is OK
    ((MTLVertexDescriptor*)_descriptor).layouts[LV_METAL_VERTEX_BUFFER_BINDING_INDEX].stepRate = 1;
    ((MTLVertexDescriptor*)_descriptor).layouts[LV_METAL_VERTEX_BUFFER_BINDING_INDEX].stepFunction = (MTLVertexStepFunction)createInfo.stepFunction;
    for (auto& binding : createInfo.bindings) {
        ((MTLVertexDescriptor*)_descriptor).attributes[binding.location].format = (MTLVertexFormat)binding.format;
        ((MTLVertexDescriptor*)_descriptor).attributes[binding.location].bufferIndex = LV_METAL_VERTEX_BUFFER_BINDING_INDEX;
        ((MTLVertexDescriptor*)_descriptor).attributes[binding.location].offset = binding.offset;
    }
}

} //namespace lv
