#include "metal/lvcore/core/temporal_aa.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

#include <iostream>

namespace lv {

void Metal_TemporalAA::init(float blendFactor) {
    temporalAA = [[MPSTemporalAA alloc] initWithDevice:g_metal_device->device()];
    temporalAA.blendFactor = blendFactor;
}

void Metal_TemporalAA::destroy() {
    [temporalAA release];
}

void Metal_TemporalAA::reset() {

}

void Metal_TemporalAA::cmdResolve(Metal_CommandBuffer* commandBuffer, Metal_Image* inputImage, Metal_Image* depthImage, Metal_Image* motionImage, Metal_Image* outputColorImage) {
    int8_t prevOutputImageIndex = (g_metal_swapChain->crntFrame() - 1) % outputColorImage->frameCount();
    if (prevOutputImageIndex < 0) prevOutputImageIndex += outputColorImage->frameCount();
    [temporalAA encodeToCommandBuffer:commandBuffer->_getActiveCommandBuffer()
                        sourceTexture:inputImage->image(g_metal_swapChain->crntFrame() % inputImage->frameCount())
                      previousTexture:outputColorImage->image(prevOutputImageIndex)
                   destinationTexture:outputColorImage->image(g_metal_swapChain->crntFrame() % outputColorImage->frameCount())
                  motionVectorTexture:motionImage->image(g_metal_swapChain->crntFrame() % motionImage->frameCount())
                         depthTexture:depthImage->image(g_metal_swapChain->crntFrame() % depthImage->frameCount())];
}

} //namespace lv
