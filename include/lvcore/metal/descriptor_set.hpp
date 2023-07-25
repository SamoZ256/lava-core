#ifndef LV_METAL_DESCRIPTOR_SET_H
#define LV_METAL_DESCRIPTOR_SET_H

#include <map>

#include "lvcore/internal/descriptor_set.hpp"

#include "pipeline_layout.hpp"
#include "sampler.hpp"
#include "buffer.hpp"

namespace lv {

namespace metal {

class DescriptorSet : public internal::DescriptorSet {
private:
    PipelineLayout* _pipelineLayout;

public:
    std::vector<std::vector<id /*MTLBuffer*/> > buffers;
    std::vector<uint32_t> bufferBindingIndices;
    std::vector<std::vector<id /*MTLTexture*/> > textures;
    std::vector<uint32_t> textureBindingIndices;
    std::vector<id /*MTLSamplerState*/> samplers;
    std::vector<uint32_t> samplerBindingIndices;

    DescriptorSet(internal::DescriptorSetCreateInfo createInfo);

    ~DescriptorSet() override {}

    //Getters
    inline PipelineLayout* pipelineLayout() { return _pipelineLayout; }
};

} //namespace metal

} //namespace lv

#endif
