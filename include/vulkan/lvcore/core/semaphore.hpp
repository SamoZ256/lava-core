#ifndef LV_VULKAN_SEMAPHORE_H
#define LV_VULKAN_SEMAPHORE_H

#include <vector>

#include <vulkan/vulkan.h>

namespace lv {

class Vulkan_Semaphore {
private:
    uint8_t _frameCount;

    std::vector<VkSemaphore> semaphores;

public:
    Vulkan_Semaphore(uint8_t frameCount = 0);

    ~Vulkan_Semaphore();

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline VkSemaphore semaphore(uint8_t index) { return semaphores[index]; }
};

} //namespace lv

#endif
