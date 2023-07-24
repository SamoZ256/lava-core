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

namespace vulkan {
 
class DescriptorWriter {
public:
    PipelineLayout* pipelineLayout;
    uint8_t layoutIndex;
    std::vector<VkWriteDescriptorSet> writes;

    DescriptorWriter(PipelineLayout* aPipelineLayout, uint8_t aLayoutIndex) : pipelineLayout(aPipelineLayout), layoutIndex(aLayoutIndex) {}
    
    void writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    void writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
    
    void build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);
};

struct DescriptorSetCreateInfo {
    uint8_t frameCount = 0;
    PipelineLayout* pipelineLayout;
    uint8_t layoutIndex = 0;
    std::vector<BufferDescriptorInfo> bufferBindings;
    std::vector<ImageDescriptorInfo> imageBindings;
};

class DescriptorSet {
private:
    uint8_t _frameCount;

    std::vector<VkDescriptorSet> descriptorSets;

    //uint16_t shaderType;
    PipelineLayout* _pipelineLayout;
    uint8_t _layoutIndex;
    VkDescriptorPool* pool;

public:
    DescriptorSet(DescriptorSetCreateInfo createInfo);

    ~DescriptorSet();

    static bool registerDescriptor(DescriptorType descriptorType);

    void registerDescriptorSet();

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline VkDescriptorSet descriptorSet(uint8_t index) { return descriptorSets[index]; }

    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }

    inline uint8_t layoutIndex() { return _layoutIndex; }
};

} //namespace vulkan

} //namespace lv

#endif
