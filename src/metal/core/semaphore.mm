#include "metal/lvcore/core/semaphore.hpp"

#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

Metal_Semaphore::Metal_Semaphore(uint8_t frameCount) {
    _frameCount = (frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : frameCount);

    _semaphore = dispatch_semaphore_create(_frameCount);
}

} //namespace lv
