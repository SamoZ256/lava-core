#ifndef LV_VULKAN_DESCRIPTOR_SET_H
#define LV_VULKAN_DESCRIPTOR_SET_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <map>
#include <iostream>

#include "buffer.hpp"
#include "sampler.hpp"
#include "pipeline_layout.hpp"

namespace lv {
 
class Vulkan_DescriptorWriter {
public:
    Vulkan_PipelineLayout* pipelineLayout;
    uint8_t layoutIndex;
    std::vector<VkWriteDescriptorSet> writes;

    Vulkan_DescriptorWriter(Vulkan_PipelineLayout* aPipelineLayout, uint8_t aLayoutIndex) : pipelineLayout(aPipelineLayout), layoutIndex(aLayoutIndex) {}
    
    void writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    void writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
    
    void build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);
};

struct Vulkan_DescriptorSetCreateInfo {
    uint8_t frameCount = 0;
    Vulkan_PipelineLayout* pipelineLayout;
    uint8_t layoutIndex = 0;
    std::vector<Vulkan_BufferDescriptorInfo> bufferBindings;
    std::vector<Vulkan_ImageDescriptorInfo> imageBindings;
};

class Vulkan_DescriptorSet {
private:
    uint8_t _frameCount;

    std::vector<VkDescriptorSet> descriptorSets;

    //uint16_t shaderType;
    Vulkan_PipelineLayout* _pipelineLayout;
    uint8_t _layoutIndex;
    VkDescriptorPool* pool;

public:
    Vulkan_DescriptorSet(Vulkan_DescriptorSetCreateInfo createInfo);

    ~Vulkan_DescriptorSet();

    static bool registerDescriptor(VkDescriptorType descriptorType);

    void registerDescriptorSet();

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline VkDescriptorSet descriptorSet(uint8_t index) { return descriptorSets[index]; }

    inline Vulkan_PipelineLayout* pipelineLayout() { return _pipelineLayout; }

    inline uint8_t layoutIndex() { return _layoutIndex; }
};

} //namespace lv

#endif
