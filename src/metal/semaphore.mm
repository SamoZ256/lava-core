#include "lvcore/metal/semaphore.hpp"

#include "lvcore/metal/swap_chain.hpp"

namespace lv {

namespace metal {

Semaphore::Semaphore(uint8_t frameCount) {
    _frameCount = (frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : frameCount);

    _semaphore = dispatch_semaphore_create(_frameCount);
}

} //namespace metal

} //namespace lv
