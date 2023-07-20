#include "metal/lvcore/core/vertex_descriptor.hpp"

#include "metal/lvcore/core/common.hpp"

#include "metal/lvcore/core/core.hpp"

namespace lv {

Metal_VertexDescriptor::Metal_VertexDescriptor(Metal_VertexDescriptorCreateInfo createInfo) {
    MTLVertexStepFunction mtlVertexStepFunction;
    GET_MTL_VERTEX_STEP_FUNCTION(createInfo.inputRate, mtlVertexStepFunction);

    _descriptor = [[MTLVertexDescriptor alloc] init];
    ((MTLVertexDescriptor*)_descriptor).layouts[LV_METAL_VERTEX_BUFFER_BINDING_INDEX].stride = createInfo.size;
    //TODO: check if this is OK
    ((MTLVertexDescriptor*)_descriptor).layouts[LV_METAL_VERTEX_BUFFER_BINDING_INDEX].stepRate = 1;
    ((MTLVertexDescriptor*)_descriptor).layouts[LV_METAL_VERTEX_BUFFER_BINDING_INDEX].stepFunction = mtlVertexStepFunction;
    for (auto& binding : createInfo.bindings) {
        MTLVertexFormat mtlVertexFormat;
        GET_MTL_VERTEX_FORMAT(binding.format, mtlVertexFormat);

        ((MTLVertexDescriptor*)_descriptor).attributes[binding.location].format = mtlVertexFormat;
        ((MTLVertexDescriptor*)_descriptor).attributes[binding.location].bufferIndex = LV_METAL_VERTEX_BUFFER_BINDING_INDEX;
        ((MTLVertexDescriptor*)_descriptor).attributes[binding.location].offset = binding.offset;
    }
}

} //namespace lv
