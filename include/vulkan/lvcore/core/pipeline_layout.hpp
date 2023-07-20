#ifndef LV_VULKAN_PIPELINE_LAYOUT_H
#define LV_VULKAN_PIPELINE_LAYOUT_H

#include <vector>

#include "enums.hpp"

namespace lv {

struct Vulkan_DescriptorSetLayoutBinding {
    uint32_t binding;
    LvDescriptorType descriptorType;
    LvShaderStageFlags shaderStage;
};

class Vulkan_DescriptorSetLayout {
public:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    Vulkan_DescriptorSetLayout(std::vector<Vulkan_DescriptorSetLayoutBinding> aBindings);
    
    void destroy(); //TODO: probably make this a destructor? Or maybe just move the whole class into pipeline layout
};

struct Vulkan_PipelineLayoutCreateInfo {
    std::vector<Vulkan_DescriptorSetLayout> descriptorSetLayouts;
    std::vector<VkPushConstantRange> pushConstantRanges;
};

class Vulkan_PipelineLayout {
private:
    VkPipelineLayout _pipelineLayout;
    std::vector<Vulkan_DescriptorSetLayout> descriptorSetLayouts;
    std::vector<VkPushConstantRange> pushConstantRanges;

public:
    Vulkan_PipelineLayout(Vulkan_PipelineLayoutCreateInfo createInfo);

    ~Vulkan_PipelineLayout();

    //Getters
    inline VkPipelineLayout pipelineLayout() { return _pipelineLayout; }

    inline Vulkan_DescriptorSetLayout& descriptorSetLayout(uint32_t index) { return descriptorSetLayouts[index]; }

    inline VkPushConstantRange& pushConstantRange(uint32_t index) { return pushConstantRanges[index]; }
};

} //namespace lv

#endif