#ifndef LV_METAL_UPSCALER_H
#define LV_METAL_UPSCALER_H

#include "command_buffer.hpp"

namespace lv {

namespace metal {

struct UpscalerCreateInfo {
    Image* inputColorImage = nullptr;
    Image* depthImage = nullptr;
    Image* motionImage = nullptr;
    Image* outputColorImage = nullptr;
    bool autoExposureEnable = false;
};

class Upscaler {
public:
    _LV_MTLFX_TEMPORAL_SCALER_T upscaler;

    Image* inputColorImage;
    Image* depthImage;
    Image* motionImage;
    Image* outputColorImage;

    void init(UpscalerCreateInfo& createInfo);

    void destroy();

    void reset();

    void cmdUpscale(CommandBuffer* commandBuffer, float jitterOffsetX, float jitterOffsetY, bool depthReversed = false);
};

} //namespace metal

} //namespace lv

#endif
