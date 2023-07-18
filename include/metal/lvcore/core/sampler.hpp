#ifndef LV_METAL_SAMPLER_H
#define LV_METAL_SAMPLER_H

#include "enums.hpp"

#include "image.hpp"

namespace lv {

struct Metal_SamplerCreateInfo {
    LvFilter filter = LV_FILTER_NEAREST;
    LvSamplerAddressMode addressMode = LV_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    LvBool compareEnable = LV_FALSE;
    LvCompareOp compareOp = LV_COMPARE_OP_LESS;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

class Metal_Sampler {
private:
    id /*MTLSamplerState*/ _sampler;

public:
    Metal_Sampler(Metal_SamplerCreateInfo createInfo);

    ~Metal_Sampler();

    Metal_ImageDescriptorInfo descriptorInfo(Metal_Image* image, uint32_t binding, LvImageLayout imageLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, int8_t frameOffset = 0);
};

} //namespace lv

#endif
