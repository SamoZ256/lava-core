#ifndef LV_METAL_UPSCALER_H
#define LV_METAL_UPSCALER_H

#include "command_buffer.hpp"

namespace lv {

struct Metal_UpscalerCreateInfo {
    Metal_Image* inputColorImage = nullptr;
    Metal_Image* depthImage = nullptr;
    Metal_Image* motionImage = nullptr;
    Metal_Image* outputColorImage = nullptr;
    bool autoExposureEnable = false;
};

class Metal_Upscaler {
public:
    _LV_MTLFX_TEMPORAL_SCALER_T upscaler;

    Metal_Image* inputColorImage;
    Metal_Image* depthImage;
    Metal_Image* motionImage;
    Metal_Image* outputColorImage;

    void init(Metal_UpscalerCreateInfo& createInfo);

    void destroy();

    void reset();

    void cmdUpscale(lv::Metal_CommandBuffer* commandBuffer, float jitterOffsetX, float jitterOffsetY, bool depthReversed = false);
};

} //namespace lv

#endif
