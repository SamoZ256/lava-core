#ifndef LV_VULKAN_PIPELINE_LAYOUT_H
#define LV_VULKAN_PIPELINE_LAYOUT_H

#include "lvcore/internal/pipeline_layout.hpp"

#include "core.hpp"

namespace lv {

namespace vulkan {

class DescriptorSetLayout {
public:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    DescriptorSetLayout(std::vector<internal::DescriptorSetLayoutBinding> aBindings);
    
    void destroy(); //TODO: probably make this a destructor? Or maybe just move the whole class into pipeline layout
};

class PipelineLayout : public internal::PipelineLayout {
private:
    VkPipelineLayout _pipelineLayout;
    std::vector<DescriptorSetLayout> descriptorSetLayouts;
    std::vector<VkPushConstantRange> pushConstantRanges;

public:
    PipelineLayout(internal::PipelineLayoutCreateInfo createInfo);

    ~PipelineLayout();

    //Getters
    inline VkPipelineLayout pipelineLayout() { return _pipelineLayout; }

    inline DescriptorSetLayout& descriptorSetLayout(uint32_t index) { return descriptorSetLayouts[index]; }

    inline VkPushConstantRange& pushConstantRange(uint32_t index) { return pushConstantRanges[index]; }
};

} //namespace vulkan

} //namespace lv

#endif
