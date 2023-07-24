#ifndef LV_VULKAN_SAMPLER_H
#define LV_VULKAN_SAMPLER_H

#include <iostream>

#include "lvcore/internal/common.hpp"

#include "image.hpp"

namespace lv {

namespace vulkan {

struct SamplerCreateInfo {
    Filter filter = Filter::Nearest;
    SamplerAddressMode addressMode = SamplerAddressMode::ClampToEdge;
    Bool compareEnable = False;
    CompareOperation compareOp = CompareOperation::Less;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

class Sampler {
private:
    VkSampler _sampler;

public:
    Sampler(SamplerCreateInfo createInfo);

    ~Sampler();

    ImageDescriptorInfo descriptorInfo(Image* image, uint32_t binding, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0);
};

} //namespace vulkan

} //namespace lv

#endif
