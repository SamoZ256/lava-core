#include "lvcore/vulkan/vertex_descriptor.hpp"

namespace lv {

namespace vulkan {

VertexDescriptor::VertexDescriptor(internal::VertexDescriptorCreateInfo createInfo) {
    bindingDescriptions.resize(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = createInfo.size;
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    attributeDescriptions.resize(createInfo.bindings.size());
    for (uint16_t i = 0; i < attributeDescriptions.size(); i++) {
        VkFormat vkFormat;
        GET_VK_FORMAT(createInfo.bindings[i].format, vkFormat);

        attributeDescriptions[i].binding = 0;
        attributeDescriptions[i].location = createInfo.bindings[i].location;
        attributeDescriptions[i].format = vkFormat;
        attributeDescriptions[i].offset = createInfo.bindings[i].offset;
    }
}

} //namespace vulkan

} //namespace lv
