#ifndef LV_METAL_SAMPLER_H
#define LV_METAL_SAMPLER_H

#include "lvcore/internal/sampler.hpp"

#include "image.hpp"

namespace lv {

namespace metal {

class Sampler : public internal::Sampler {
private:
    id /*MTLSamplerState*/ _sampler;

public:
    Sampler(internal::SamplerCreateInfo createInfo);

    ~Sampler() override;

    internal::ImageDescriptorInfo* descriptorInfo(internal::Image* image, uint32_t binding, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0) override;
};

} //namespace metal

} //namespace lv

#endif
