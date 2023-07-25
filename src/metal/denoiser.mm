#include "lvcore/metal/denoiser.hpp"

#include "lvcore/metal/device.hpp"
#include "lvcore/metal/swap_chain.hpp"

namespace lv {

namespace metal {

void Denoiser::init(DenoiserCreateInfo& createInfo) {
    textureAllocator = [[MPSSVGFDefaultTextureAllocator alloc] initWithDevice:g_metal_device->device()];
    
    MPSSVGF* svgf = [[MPSSVGF alloc] initWithDevice:g_metal_device->device()];
    
    svgf.channelCount = createInfo.channelCount;
    
    svgf.temporalWeighting = MPSTemporalWeightingExponentialMovingAverage;
    svgf.temporalReprojectionBlendFactor = createInfo.temporalReprojectionBlendFactor;
    
    denoiser = [[MPSSVGFDenoiser alloc] initWithSVGF:svgf textureAllocator:textureAllocator];
    
    denoiser.bilateralFilterIterations = createInfo.bilateralFilterPassCount;
}

void Denoiser::destroy() {
    [textureAllocator release];
    [denoiser release];
}

void Denoiser::reset() {

}

void Denoiser::cmdDenoise(CommandBuffer* commandBuffer, Image* inputImage, Image* normalDepthImage, Image* motionImage, Image* outputColorImage) {
    int8_t prevNormalDepthImageIndex = (g_metal_swapChain->crntFrame() - 1) % normalDepthImage->frameCount();
    if (prevNormalDepthImageIndex < 0) prevNormalDepthImageIndex += normalDepthImage->frameCount();
    outputColorImage->_setImage([denoiser encodeToCommandBuffer:commandBuffer->_getActiveCommandBuffer()
                                 sourceTexture:inputImage->image(g_metal_swapChain->crntFrame() % inputImage->frameCount())
                           motionVectorTexture:motionImage->image(g_metal_swapChain->crntFrame() % motionImage->frameCount())
                            depthNormalTexture:normalDepthImage->image(g_metal_swapChain->crntFrame() % normalDepthImage->frameCount())
                    previousDepthNormalTexture:normalDepthImage->image(prevNormalDepthImageIndex)],
                    g_metal_swapChain->crntFrame() % outputColorImage->frameCount());
}

} //namespace metal

} //namespace lv
