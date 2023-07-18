#ifndef LV_THREAD_POOL_H
#define LV_THREAD_POOL_H

#include <vector>

#include "thread.hpp"

namespace lv {

struct ThreadPoolCreateInfo {
    uint8_t maxThreadCount = 8;
};

class ThreadPool {
private:
    std::vector<uint8_t> threadIndicesQuery;

    uint8_t _maxThreadCount;

public:
    ThreadPool(ThreadPoolCreateInfo createInfo);

    ~ThreadPool() {}

    Thread* getNextAvailableThread();

    //Getters
    inline uint8_t maxThreadCount() { return _maxThreadCount; }
};

} //namespace lv

#endif
