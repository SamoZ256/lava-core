#ifndef LV_METAL_DENOISER_H
#define LV_METAL_DENOISER_H

#include "command_buffer.hpp"

namespace lv {

namespace metal {

class TemporalAA {
private:
    _LV_MPS_TEMPORAL_AA_T temporalAA;

public:
    void init(float blendFactor = 0.1f);

    void destroy();

    void reset();

    void cmdResolve(CommandBuffer* commandBuffer, Image* inputImage, Image* depthImage, Image* motionImage, Image* outputColorImage);
};

} //namespace metal

} //namespace lv

#endif
