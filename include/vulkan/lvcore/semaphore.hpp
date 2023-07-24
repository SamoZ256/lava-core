#ifndef LV_VULKAN_SEMAPHORE_H
#define LV_VULKAN_SEMAPHORE_H

#include <vulkan/vulkan.h>

#include "lvcore/internal/semaphore.hpp"

namespace lv {

namespace vulkan {

class Semaphore : public internal::Semaphore {
private:
    std::vector<VkSemaphore> semaphores;

public:
    Semaphore(uint8_t frameCount = 0);

    ~Semaphore() override;

    //Getters
    inline VkSemaphore semaphore(uint8_t index) { return semaphores[index]; }
};

} //namespace vulkan

} //namespace lv

#endif
