#ifndef LV_INTERNAL_PIPELINE_LAYOUT_H
#define LV_INTERNAL_PIPELINE_LAYOUT_H

#include "common.hpp"

namespace lv {

namespace internal {

struct PushConstantRange {
    ShaderStageFlags stageFlags;
    uint32_t offset;
    uint32_t size;
};

struct DescriptorSetLayoutBinding {
    uint32_t binding;
    DescriptorType descriptorType;
    ShaderStageFlags shaderStage;
};

struct PipelineLayoutCreateInfo {
    std::vector<std::vector<internal::DescriptorSetLayoutBinding> > descriptorSetLayouts;
    std::vector<PushConstantRange> pushConstantRanges;
};

class PipelineLayout {
public:
    virtual ~PipelineLayout() {}
};

} //namespace internal

} //namespace lv

#endif
