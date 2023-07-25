#ifndef LV_METAL_VIEWPORT_H
#define LV_METAL_VIEWPORT_H

#include "common.hpp"

namespace lv {

namespace metal {

class Viewport {
public:
    void* /**MTLViewport*/ viewport;
    void* /**MTLScissorRect*/ scissor;
    uint32_t totalWidth = 0;
    uint32_t totalHeight = 0;

    Viewport() {}

    Viewport(int32_t x, int32_t y, uint32_t width, uint32_t height);

    void setViewport(int32_t x, int32_t y, uint32_t width, uint32_t height);

	void setTotalSize(uint32_t aTotalWidth, uint32_t aTotalHeight) { totalWidth = aTotalWidth; totalHeight = aTotalHeight; }
};

} //namespace metal

} //namespace lv

#endif
