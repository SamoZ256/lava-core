#ifndef LV_VULKAN_SAMPLER_H
#define LV_VULKAN_SAMPLER_H

#include <iostream>

#include "image.hpp"

namespace lv {

struct Vulkan_SamplerCreateInfo {
    LvFilter filter = LV_FILTER_NEAREST;
    LvSamplerAddressMode addressMode = LV_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    LvBool compareEnable = LV_FALSE;
    LvCompareOp compareOp = LV_COMPARE_OP_LESS;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

class Vulkan_Sampler {
private:
    VkSampler _sampler;

public:
    Vulkan_Sampler(Vulkan_SamplerCreateInfo createInfo);

    ~Vulkan_Sampler();

    Vulkan_ImageDescriptorInfo descriptorInfo(Vulkan_Image* image, uint32_t binding, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, int8_t frameOffset = 0);
};

} //namespace lv

#endif
