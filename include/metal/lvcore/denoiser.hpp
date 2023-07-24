#ifndef LV_METAL_DENOISER_H
#define LV_METAL_DENOISER_H

#include "command_buffer.hpp"

namespace lv {

namespace metal {

struct DenoiserCreateInfo {
    uint8_t channelCount = 3;
    uint8_t bilateralFilterPassCount = 2;
    float temporalReprojectionBlendFactor = 0.1f;
};

class Denoiser {
private:
    _LV_MPS_SVGF_DENOISER_T denoiser;
    id /*MPSSVGFDefaultTextureAllocator*/ textureAllocator;

public:
    void init(DenoiserCreateInfo& createInfo);

    void destroy();

    void reset();

    void cmdDenoise(CommandBuffer* commandBuffer, Image* inputImage, Image* normalDepthImage, Image* motionImage, Image* outputColorImage);

    //Getters
    inline _LV_MPS_SVGF_DENOISER_T getDenoiser() { return denoiser; }
};

} //namespace metal

} //namespace lv

#endif
