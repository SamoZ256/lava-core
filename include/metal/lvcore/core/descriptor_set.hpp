#ifndef LV_METAL_DESCRIPTOR_SET_H
#define LV_METAL_DESCRIPTOR_SET_H

#include <map>

#include "pipeline_layout.hpp"
#include "sampler.hpp"
#include "buffer.hpp"

namespace lv {

struct Metal_DescriptorSetCreateInfo {
    uint8_t frameCount = 0;
    Metal_PipelineLayout* pipelineLayout;
    uint8_t layoutIndex = 0;
    std::vector<Metal_BufferDescriptorInfo> bufferBindings;
    std::vector<Metal_ImageDescriptorInfo> imageBindings;
};

class Metal_DescriptorSet {
private:
    uint8_t _frameCount;

    Metal_PipelineLayout* _pipelineLayout;
    uint8_t _layoutIndex;

public:
    std::vector<std::vector<id /*MTLBuffer*/> > buffers;
    std::vector<uint32_t> bufferBindingIndices;
    std::vector<std::vector<id /*MTLTexture*/> > textures;
    std::vector<uint32_t> textureBindingIndices;
    std::vector<id /*MTLSamplerState*/> samplers;
    std::vector<uint32_t> samplerBindingIndices;

    Metal_DescriptorSet(Metal_DescriptorSetCreateInfo createInfo);

    ~Metal_DescriptorSet() {}

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline Metal_PipelineLayout* pipelineLayout() { return _pipelineLayout; }

    inline uint8_t layoutIndex() { return _layoutIndex; }
};

} //namespace lv

#endif
