#ifndef LV_METAL_PIPELINE_LAYOUT_H
#define LV_METAL_PIPELINE_LAYOUT_H

#include <vector>

#include "lvcore/core/core.hpp"

#include "enums.hpp"

namespace lv {

struct Metal_SortedDescriptorSetLayoutBinding {
    DescriptorType descriptorType;
    ShaderStageFlags shaderStage;
};

struct Metal_PushConstantRange {
    ShaderStageFlags stageFlags;
    uint32_t offset;
    uint32_t size;
};

struct Metal_DescriptorSetLayoutBinding {
    uint32_t binding;
    DescriptorType descriptorType;
    ShaderStageFlags shaderStage;
};

class Metal_DescriptorSetLayout {
public:
    std::vector<Metal_SortedDescriptorSetLayoutBinding> bindings;

    Metal_DescriptorSetLayout(std::vector<Metal_DescriptorSetLayoutBinding> aBindings) {
        uint32_t bindingCount = 0;
        for (auto& binding : aBindings)
            bindingCount = std::max(bindingCount, binding.binding + 1);
        bindings.resize(bindingCount);
        for (auto& binding : aBindings)
            bindings[binding.binding] = {binding.descriptorType, binding.shaderStage};
    }
    
    ~Metal_DescriptorSetLayout() {}
};

struct Metal_PipelineLayoutCreateInfo {
    std::vector<Metal_DescriptorSetLayout> descriptorSetLayouts;
    std::vector<Metal_PushConstantRange> pushConstantRanges;
};

class Metal_PipelineLayout {
public:
    std::vector<Metal_DescriptorSetLayout> descriptorSetLayouts;
    std::vector<Metal_PushConstantRange> pushConstantRanges;

    Metal_PipelineLayout(Metal_PipelineLayoutCreateInfo createInfo) {
        descriptorSetLayouts = createInfo.descriptorSetLayouts;
        pushConstantRanges = createInfo.pushConstantRanges;
    }

    ~Metal_PipelineLayout() {}
};

} //namespace lv

#endif
