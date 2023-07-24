#ifndef LV_VULKAN_VERTEX_DESCRIPTOR_H
#define LV_VULKAN_VERTEX_DESCRIPTOR_H

#include <vector>

#include "core.hpp"

namespace lv {

namespace vulkan {

struct VertexDescriptorBinding {
    uint16_t location;
    Format format;
    uint32_t offset;
};

struct VertexDescriptorCreateInfo {
    size_t size;
    std::vector<VertexDescriptorBinding> bindings;
};

class VertexDescriptor {
private:
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

public:
    VertexDescriptor(VertexDescriptorCreateInfo createInfo);

    //Getters
    inline size_t bindingDescriptionCount() { return bindingDescriptions.size(); }

    inline VkVertexInputBindingDescription* bindingDescriptionsData() { return bindingDescriptions.data(); }

    inline size_t attributeDescriptionCount() { return attributeDescriptions.size(); }

    inline VkVertexInputAttributeDescription* attributeDescriptionsData() { return attributeDescriptions.data(); }
};

} //namespace vulkan

} //namespace lv

#endif
