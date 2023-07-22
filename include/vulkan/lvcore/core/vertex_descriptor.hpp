#ifndef LV_VULKAN_VERTEX_DESCRIPTOR_H
#define LV_VULKAN_VERTEX_DESCRIPTOR_H

#include <vector>

#include "core.hpp"

namespace lv {

struct Vulkan_VertexDescriptorBinding {
    uint16_t location;
    Format format;
    uint32_t offset;
};

struct Vulkan_VertexDescriptorCreateInfo {
    size_t size;
    std::vector<Vulkan_VertexDescriptorBinding> bindings;
};

class Vulkan_VertexDescriptor {
private:
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

public:
    Vulkan_VertexDescriptor(Vulkan_VertexDescriptorCreateInfo createInfo);

    //Getters
    inline size_t bindingDescriptionCount() { return bindingDescriptions.size(); }

    inline VkVertexInputBindingDescription* bindingDescriptionsData() { return bindingDescriptions.data(); }

    inline size_t attributeDescriptionCount() { return attributeDescriptions.size(); }

    inline VkVertexInputAttributeDescription* attributeDescriptionsData() { return attributeDescriptions.data(); }
};

} //namespace lv

#endif
