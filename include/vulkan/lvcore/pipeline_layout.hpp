#ifndef LV_VULKAN_PIPELINE_LAYOUT_H
#define LV_VULKAN_PIPELINE_LAYOUT_H

#include <vector>

#include "core.hpp"

namespace lv {

namespace vulkan {

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

//TODO: use contructors and destructors
class DescriptorSetLayout {
public:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    DescriptorSetLayout(std::vector<DescriptorSetLayoutBinding> aBindings);
    
    void destroy(); //TODO: probably make this a destructor? Or maybe just move the whole class into pipeline layout
};

struct PipelineLayoutCreateInfo {
    std::vector<DescriptorSetLayout> descriptorSetLayouts;
    std::vector<PushConstantRange> pushConstantRanges;
};

class PipelineLayout {
private:
    VkPipelineLayout _pipelineLayout;
    std::vector<DescriptorSetLayout> descriptorSetLayouts;
    std::vector<VkPushConstantRange> pushConstantRanges;

public:
    PipelineLayout(PipelineLayoutCreateInfo createInfo);

    ~PipelineLayout();

    //Getters
    inline VkPipelineLayout pipelineLayout() { return _pipelineLayout; }

    inline DescriptorSetLayout& descriptorSetLayout(uint32_t index) { return descriptorSetLayouts[index]; }

    inline VkPushConstantRange& pushConstantRange(uint32_t index) { return pushConstantRanges[index]; }
};

} //namespace vulkan

} //namespace lv

#endif
