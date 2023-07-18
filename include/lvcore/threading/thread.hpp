#ifndef LV_THREAD_H
#define LV_THREAD_H

#include <thread>
#include <functional>

namespace lv {

class Thread {
public:
    std::thread* thread;

    uint8_t threadIndex;
    bool isAvailable;

    Thread(uint8_t aThreadIndex, bool aIsAvailable) : threadIndex(aThreadIndex), isAvailable(aIsAvailable) {}

    void dispatch(std::function<void(uint8_t)> function);
};

} //namespace lv

#endif
