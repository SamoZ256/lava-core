#ifndef LV_VULKAN_SAMPLER_H
#define LV_VULKAN_SAMPLER_H

#include <iostream>

#include "lvcore/core/common.hpp"

#include "image.hpp"

namespace lv {

struct Vulkan_SamplerCreateInfo {
    Filter filter = Filter::Nearest;
    SamplerAddressMode addressMode = SamplerAddressMode::ClampToEdge;
    Bool compareEnable = False;
    CompareOperation compareOp = CompareOperation::Less;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

class Vulkan_Sampler {
private:
    VkSampler _sampler;

public:
    Vulkan_Sampler(Vulkan_SamplerCreateInfo createInfo);

    ~Vulkan_Sampler();

    Vulkan_ImageDescriptorInfo descriptorInfo(Vulkan_Image* image, uint32_t binding, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0);
};

} //namespace lv

#endif
