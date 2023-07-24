#ifndef LV_INTERNAL_SEMAPHORE_H
#define LV_INTERNAL_SEMAPHORE_H

#include "common.hpp"

namespace lv {

namespace internal {

class Semaphore {
protected:
    uint8_t _frameCount;

public:
    virtual ~Semaphore() {}
};

} //namespace internal

} //namespace lv

#endif
