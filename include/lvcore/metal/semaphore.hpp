#ifndef LV_METAL_SEMAPHORE_H
#define LV_METAL_SEMAPHORE_H

#include "lvcore/internal/semaphore.hpp"

#include "common.hpp"

namespace lv {

namespace metal {

class Semaphore : public internal::Semaphore {
private:
    void* /*dispatch_semaphore_t*/ _semaphore;

public:
    Semaphore(uint8_t frameCount = 0);

    ~Semaphore() override {}

    //Getters
    inline void* /*dispatch_semaphore_t*/ semaphore() { return _semaphore; }
};

} //namespace metal

} //namespace lv

#endif
