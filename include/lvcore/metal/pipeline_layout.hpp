#ifndef LV_METAL_PIPELINE_LAYOUT_H
#define LV_METAL_PIPELINE_LAYOUT_H

#include "lvcore/internal/pipeline_layout.hpp"

namespace lv {

namespace metal {

struct SortedDescriptorSetLayoutBinding {
    DescriptorType descriptorType;
    ShaderStageFlags shaderStage;
};

class DescriptorSetLayout {
public:
    std::vector<SortedDescriptorSetLayoutBinding> bindings;

    DescriptorSetLayout(std::vector<internal::DescriptorSetLayoutBinding> aBindings) {
        uint32_t bindingCount = 0;
        for (auto& binding : aBindings)
            bindingCount = std::max(bindingCount, binding.binding + 1);
        bindings.resize(bindingCount);
        for (auto& binding : aBindings)
            bindings[binding.binding] = {binding.descriptorType, binding.shaderStage};
    }
    
    ~DescriptorSetLayout() {}
};

class PipelineLayout : public internal::PipelineLayout {
public:
    std::vector<DescriptorSetLayout> descriptorSetLayouts;
    std::vector<internal::PushConstantRange> pushConstantRanges;

    PipelineLayout(internal::PipelineLayoutCreateInfo createInfo) {
        descriptorSetLayouts.reserve(createInfo.descriptorSetLayouts.size());
        for (uint32_t i = 0; i < createInfo.descriptorSetLayouts.size(); i++)
            descriptorSetLayouts.emplace_back(createInfo.descriptorSetLayouts[i]);
        pushConstantRanges = createInfo.pushConstantRanges;
    }

    ~PipelineLayout() override {}
};

} //namespace metal

} //namespace lv

#endif
