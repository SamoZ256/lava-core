#ifndef LV_INTERNAL_VIEWPORT_H
#define LV_INTERNAL_VIEWPORT_H

#include "common.hpp"

namespace lv {

namespace internal {

class Viewport {
protected:
	uint32_t totalWidth = 0;
	uint32_t totalHeight = 0;

public:
    virtual void setViewport(int32_t x, int32_t y, uint32_t width, uint32_t height) = 0;

	void setTotalSize(uint32_t aTotalWidth, uint32_t aTotalHeight) { totalWidth = aTotalWidth; totalHeight = aTotalHeight; }
};

} //namespace internal

} //namespace lv

#endif
