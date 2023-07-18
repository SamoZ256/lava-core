#ifndef LV_METAL_DENOISER_H
#define LV_METAL_DENOISER_H

#include "command_buffer.hpp"

namespace lv {

class Metal_TemporalAA {
private:
    _LV_MPS_TEMPORAL_AA_T temporalAA;

public:
    void init(float blendFactor = 0.1f);

    void destroy();

    void reset();

    void cmdResolve(Metal_CommandBuffer* commandBuffer, Metal_Image* inputImage, Metal_Image* depthImage, Metal_Image* motionImage, Metal_Image* outputColorImage);
};

} //namespace lv

#endif
