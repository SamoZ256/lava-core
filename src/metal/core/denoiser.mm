#include "metal/lvcore/core/denoiser.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

void Metal_Denoiser::init(Metal_DenoiserCreateInfo& createInfo) {
    textureAllocator = [[MPSSVGFDefaultTextureAllocator alloc] initWithDevice:g_metal_device->device()];
    
    MPSSVGF* svgf = [[MPSSVGF alloc] initWithDevice:g_metal_device->device()];
    
    svgf.channelCount = createInfo.channelCount;
    
    svgf.temporalWeighting = MPSTemporalWeightingExponentialMovingAverage;
    svgf.temporalReprojectionBlendFactor = createInfo.temporalReprojectionBlendFactor;
    
    denoiser = [[MPSSVGFDenoiser alloc] initWithSVGF:svgf textureAllocator:textureAllocator];
    
    denoiser.bilateralFilterIterations = createInfo.bilateralFilterPassCount;
}

void Metal_Denoiser::destroy() {
    [textureAllocator release];
    [denoiser release];
}

void Metal_Denoiser::reset() {

}

void Metal_Denoiser::cmdDenoise(Metal_CommandBuffer* commandBuffer, Metal_Image* inputImage, Metal_Image* normalDepthImage, Metal_Image* motionImage, Metal_Image* outputColorImage) {
    int8_t prevNormalDepthImageIndex = (g_metal_swapChain->crntFrame() - 1) % normalDepthImage->frameCount();
    if (prevNormalDepthImageIndex < 0) prevNormalDepthImageIndex += normalDepthImage->frameCount();
    outputColorImage->_setImage([denoiser encodeToCommandBuffer:commandBuffer->_getActiveCommandBuffer()
                                 sourceTexture:inputImage->image(g_metal_swapChain->crntFrame() % inputImage->frameCount())
                           motionVectorTexture:motionImage->image(g_metal_swapChain->crntFrame() % motionImage->frameCount())
                            depthNormalTexture:normalDepthImage->image(g_metal_swapChain->crntFrame() % normalDepthImage->frameCount())
                    previousDepthNormalTexture:normalDepthImage->image(prevNormalDepthImageIndex)],
                    g_metal_swapChain->crntFrame() % outputColorImage->frameCount());
}

} //namespace lv
