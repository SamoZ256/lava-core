#ifndef LV_INTERNAL_SWAP_CHAIN_H
#define LV_INTERNAL_SWAP_CHAIN_H

#include "common.hpp"

#define LVND_BACKEND_VULKAN
#ifdef __APPLE__
#define LVND_BACKEND_METAL
#endif
#include "lvnd/lvnd.h"

namespace lv {

namespace internal {

struct SwapChainCreateInfo {
	LvndWindow* window;
  	Bool vsyncEnable = True;
	uint8_t maxFramesInFlight = 2;
	Bool clearAttachment = False;
    Bool createDepthAttachment = False;
};

class SwapChain {
protected:
    uint8_t _maxFramesInFlight;
    uint8_t _crntFrame = 0;

public:
    virtual ~SwapChain() {}

    virtual void create() = 0;

    virtual void resize() = 0;

    virtual void acquireNextImage() = 0;

    virtual void renderAndPresent() = 0;

    //Getters
    inline uint8_t maxFramesInFlight() { return _maxFramesInFlight; }

    inline uint8_t crntFrame() { return _crntFrame; }
};

} //namespace internal

} //namespace lv

#endif
