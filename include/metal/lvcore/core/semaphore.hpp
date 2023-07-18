#ifndef LV_METAL_SEMAPHORE_H
#define LV_METAL_SEMAPHORE_H

#include "common.hpp"

namespace lv {

class Metal_Semaphore {
private:
    uint8_t _frameCount;

    void* /*dispatch_semaphore_t*/ _semaphore;

public:
    Metal_Semaphore(uint8_t frameCount = 0);

    ~Metal_Semaphore() {}

    //Getters
    inline void* /*dispatch_semaphore_t*/ semaphore() { return _semaphore; }
};

} //namespace lv

#endif
