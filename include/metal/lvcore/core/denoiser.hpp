#ifndef LV_METAL_DENOISER_H
#define LV_METAL_DENOISER_H

#include "command_buffer.hpp"

namespace lv {

struct Metal_DenoiserCreateInfo {
    uint8_t channelCount = 3;
    uint8_t bilateralFilterPassCount = 2;
    float temporalReprojectionBlendFactor = 0.1f;
};

class Metal_Denoiser {
private:
    _LV_MPS_SVGF_DENOISER_T denoiser;
    id /*MPSSVGFDefaultTextureAllocator*/ textureAllocator;

public:
    void init(Metal_DenoiserCreateInfo& createInfo);

    void destroy();

    void reset();

    void cmdDenoise(Metal_CommandBuffer* commandBuffer, Metal_Image* inputImage, Metal_Image* normalDepthImage, Metal_Image* motionImage, Metal_Image* outputColorImage);

    //Getters
    inline _LV_MPS_SVGF_DENOISER_T getDenoiser() { return denoiser; }
};

} //namespace lv

#endif
