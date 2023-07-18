#include "vulkan/lvcore/core/buffer.hpp"

#include "vulkan/lvcore/core/buffer_helper.hpp"
#include "vulkan/lvcore/core/swap_chain.hpp"

namespace lv {

Vulkan_Buffer::Vulkan_Buffer(Vulkan_BufferCreateInfo createInfo) : _size(createInfo.size) {
    _frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

    buffers.resize(_frameCount);
    allocations.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++)
        allocations[i] = Vulkan_BufferHelper::createBuffer(_size, createInfo.usage, buffers[i], nullptr, createInfo.memoryType, createInfo.memoryAllocationFlags); //VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
}

Vulkan_Buffer::~Vulkan_Buffer() {
    for (uint8_t i = 0; i < _frameCount; i++)
        vmaDestroyBuffer(g_vulkan_device->allocator(), buffers[i], allocations[i]);
}

void Vulkan_Buffer::copyDataTo(void* data, size_t aSize) {
    if (aSize == 0)
        aSize = _size;
    uint8_t index = g_vulkan_swapChain->crntFrame() % _frameCount;

    void* mappedData;
    vmaMapMemory(g_vulkan_device->allocator(), allocations[index], &mappedData);
    memcpy(mappedData, data, aSize);
    vmaUnmapMemory(g_vulkan_device->allocator(), allocations[index]);
}

Vulkan_BufferDescriptorInfo Vulkan_Buffer::descriptorInfo(uint32_t binding, LvDescriptorType descriptorType/*VkDeviceSize size, VkDeviceSize offset*/) {
	Vulkan_BufferDescriptorInfo info;
	info.infos.resize(_frameCount);
	for (uint8_t i = 0; i < _frameCount; i++) {
		info.infos[i].buffer = buffers[i];
		info.infos[i].offset = 0;//offset;
		info.infos[i].range = VK_WHOLE_SIZE;//size;
	}
    info.binding = binding;
	info.descriptorType = descriptorType;

	return info;
}

} //namespace lv
