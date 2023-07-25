#ifndef LV_VULKAN_DESCRIPTOR_SET_H
#define LV_VULKAN_DESCRIPTOR_SET_H

#include "lvcore/internal/descriptor_set.hpp"

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

class DescriptorSet : public internal::DescriptorSet {
private:
    std::vector<VkDescriptorSet> descriptorSets;

    PipelineLayout* _pipelineLayout;
    VkDescriptorPool* pool;

public:
    DescriptorSet(internal::DescriptorSetCreateInfo createInfo);

    ~DescriptorSet() override;

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
