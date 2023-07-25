#include "lvcore/metal/temporal_aa.hpp"

#include "lvcore/metal/device.hpp"
#include "lvcore/metal/swap_chain.hpp"

#include <iostream>

namespace lv {

namespace metal {

void TemporalAA::init(float blendFactor) {
    temporalAA = [[MPSTemporalAA alloc] initWithDevice:g_metal_device->device()];
    temporalAA.blendFactor = blendFactor;
}

void TemporalAA::destroy() {
    [temporalAA release];
}

void TemporalAA::reset() {

}

void TemporalAA::cmdResolve(CommandBuffer* commandBuffer, Image* inputImage, Image* depthImage, Image* motionImage, Image* outputColorImage) {
    int8_t prevOutputImageIndex = (g_metal_swapChain->crntFrame() - 1) % outputColorImage->frameCount();
    if (prevOutputImageIndex < 0) prevOutputImageIndex += outputColorImage->frameCount();
    [temporalAA encodeToCommandBuffer:commandBuffer->_getActiveCommandBuffer()
                        sourceTexture:inputImage->image(g_metal_swapChain->crntFrame() % inputImage->frameCount())
                      previousTexture:outputColorImage->image(prevOutputImageIndex)
                   destinationTexture:outputColorImage->image(g_metal_swapChain->crntFrame() % outputColorImage->frameCount())
                  motionVectorTexture:motionImage->image(g_metal_swapChain->crntFrame() % motionImage->frameCount())
                         depthTexture:depthImage->image(g_metal_swapChain->crntFrame() % depthImage->frameCount())];
}

} //namespace metal

} //namespace lv
