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

    //Getters
    inline uint8_t frameCount() { return _frameCount; }
};

} //namespace internal

} //namespace lv

#endif
