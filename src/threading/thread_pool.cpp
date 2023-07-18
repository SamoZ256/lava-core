#include "lvcore/threading/thread_pool.hpp"

namespace lv {

ThreadPool::ThreadPool(ThreadPoolCreateInfo createInfo) {
    _maxThreadCount = createInfo.maxThreadCount;
    threadIndicesQuery.resize(_maxThreadCount);
    for (uint8_t i = 1; i < _maxThreadCount; i++) {
        threadIndicesQuery[i] = i;
    }
}

Thread* ThreadPool::getNextAvailableThread() {
    if (threadIndicesQuery.size() > 0) {
        uint8_t threadIndex = threadIndicesQuery[0];
        threadIndicesQuery.erase(threadIndicesQuery.begin());

        return new Thread(threadIndex, true);
    } else {
        return new Thread(0, false);
    }
}

} //namespace lv
