#ifndef LV_METAL_SAMPLER_H
#define LV_METAL_SAMPLER_H

#include "enums.hpp"

#include "image.hpp"

namespace lv {

struct Metal_SamplerCreateInfo {
    Filter filter = Filter::Nearest;
    SamplerAddressMode addressMode = SamplerAddressMode::ClampToEdge;
    Bool compareEnable = False;
    CompareOperation compareOp = CompareOperation::Less;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

class Metal_Sampler {
private:
    id /*MTLSamplerState*/ _sampler;

public:
    Metal_Sampler(Metal_SamplerCreateInfo createInfo);

    ~Metal_Sampler();

    Metal_ImageDescriptorInfo descriptorInfo(Metal_Image* image, uint32_t binding, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0);
};

} //namespace lv

#endif
