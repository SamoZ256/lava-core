#include "lvcore/vulkan/semaphore.hpp"

#include "lvcore/vulkan/device.hpp"
#include "lvcore/vulkan/swap_chain.hpp"

namespace lv {

namespace vulkan {

Semaphore::Semaphore(uint8_t frameCount) {
    _frameCount = (frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : frameCount);

    VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    semaphores.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++)
        VK_CHECK_RESULT(vkCreateSemaphore(g_vulkan_device->device(), &semaphoreInfo, nullptr, &semaphores[i]));
}

Semaphore::~Semaphore() {
    for (uint8_t i = 0; i < _frameCount; i++)
        vkDestroySemaphore(g_vulkan_device->device(), semaphores[i], nullptr);
}

} //namespace vulkan

} //namespace lv
