#ifndef LV_INTERNAL_SAMPLER_H
#define LV_INTERNAL_SAMPLER_H

#include "image.hpp"

namespace lv {

namespace internal {

struct SamplerCreateInfo {
    Filter filter = Filter::Nearest;
    SamplerAddressMode addressMode = SamplerAddressMode::ClampToEdge;
    Bool compareEnable = False;
    CompareOperation compareOp = CompareOperation::Less;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

class Sampler {
public:
    virtual ~Sampler() {}

    virtual ImageDescriptorInfo* descriptorInfo(Image* image, uint32_t binding, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0) = 0;
};

} //namespace internal

} //namespace lv

#endif
