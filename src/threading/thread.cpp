#include "lvcore/threading/thread.hpp"

namespace lv {

void Thread::dispatch(std::function<void(uint8_t)> function) {
    if (isAvailable)
        thread = new std::thread(function, threadIndex);
    else
        function(threadIndex);
}

} //namespace lv
