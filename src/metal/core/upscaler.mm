#include "metal/lvcore/core/upscaler.hpp"

#import <MetalFX/MetalFX.h>

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

void Metal_Upscaler::init(Metal_UpscalerCreateInfo& createInfo) {
    inputColorImage = createInfo.inputColorImage;
    depthImage = createInfo.depthImage;
    motionImage = createInfo.motionImage;
    outputColorImage = createInfo.outputColorImage;

    MTLFXTemporalScalerDescriptor* descriptor = [[MTLFXTemporalScalerDescriptor alloc] init];
    descriptor.inputWidth = inputColorImage->width();
    descriptor.inputHeight = inputColorImage->height();
    descriptor.outputWidth = outputColorImage->width();
    descriptor.outputHeight = outputColorImage->height();
    descriptor.colorTextureFormat = (MTLPixelFormat)inputColorImage->format();
    descriptor.depthTextureFormat = (MTLPixelFormat)depthImage->format();
    descriptor.motionTextureFormat = (MTLPixelFormat)motionImage->format();
    descriptor.outputTextureFormat = (MTLPixelFormat)outputColorImage->format();
    descriptor.autoExposureEnabled = createInfo.autoExposureEnable;

    upscaler = [descriptor newTemporalScalerWithDevice:g_metal_device->device()];
    upscaler.motionVectorScaleX = inputColorImage->width();
    upscaler.motionVectorScaleY = -inputColorImage->height();
}

void Metal_Upscaler::destroy() {
    [upscaler release];
}

void Metal_Upscaler::reset() {
    [upscaler reset];
}

void Metal_Upscaler::cmdUpscale(lv::Metal_CommandBuffer* commandBuffer, float jitterOffsetX, float jitterOffsetY, bool depthReversed) {
    upscaler.colorTexture = inputColorImage->image(g_metal_swapChain->crntFrame() % inputColorImage->frameCount());
    upscaler.depthTexture = depthImage->image(g_metal_swapChain->crntFrame() % depthImage->frameCount());
    upscaler.motionTexture = motionImage->image(g_metal_swapChain->crntFrame() % motionImage->frameCount());
    upscaler.outputTexture = outputColorImage->image(g_metal_swapChain->crntFrame() % outputColorImage->frameCount());
    upscaler.jitterOffsetX = jitterOffsetX;
    upscaler.jitterOffsetY = jitterOffsetY;
    upscaler.depthReversed = depthReversed;

    [upscaler encodeToCommandBuffer:commandBuffer->_getActiveCommandBuffer()];
}

} //namespace lv
