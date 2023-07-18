#include "vulkan/lvcore/core/vertex_descriptor.hpp"

namespace lv {

Vulkan_VertexDescriptor::Vulkan_VertexDescriptor(Vulkan_VertexDescriptorCreateInfo createInfo) {
    bindingDescriptions.resize(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = createInfo.size;
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    attributeDescriptions.resize(createInfo.bindings.size());
    for (uint16_t i = 0; i < attributeDescriptions.size(); i++) {
        attributeDescriptions[i].binding = 0;
        attributeDescriptions[i].location = createInfo.bindings[i].location;
        attributeDescriptions[i].format = createInfo.bindings[i].format;
        attributeDescriptions[i].offset = createInfo.bindings[i].offset;
    }
}

} //namespace lv
