#ifndef LV_VULKAN_VERTEX_DESCRIPTOR_H
#define LV_VULKAN_VERTEX_DESCRIPTOR_H

#include "lvcore/internal/vertex_descriptor.hpp"

#include "core.hpp"

namespace lv {

namespace vulkan {

class VertexDescriptor : public internal::VertexDescriptor {
private:
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

public:
    VertexDescriptor(internal::VertexDescriptorCreateInfo createInfo);

    //Getters
    inline size_t bindingDescriptionCount() { return bindingDescriptions.size(); }

    inline VkVertexInputBindingDescription* bindingDescriptionsData() { return bindingDescriptions.data(); }

    inline size_t attributeDescriptionCount() { return attributeDescriptions.size(); }

    inline VkVertexInputAttributeDescription* attributeDescriptionsData() { return attributeDescriptions.data(); }
};

} //namespace vulkan

} //namespace lv

#endif
