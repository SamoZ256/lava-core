#include "vulkan/lvcore/buffer.hpp"

#include "vulkan/lvcore/buffer_helper.hpp"
#include "vulkan/lvcore/swap_chain.hpp"

namespace lv {

namespace vulkan {

Buffer::Buffer(BufferCreateInfo createInfo) : _size(createInfo.size) {
    _frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

    VkMemoryPropertyFlags vkMemoryPropertyFlags;
    GET_VK_MEMORY_PROPERTIES(createInfo.memoryType, vkMemoryPropertyFlags);

    buffers.resize(_frameCount);
    allocations.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++)
        allocations[i] = BufferHelper::createBuffer(_size, getVKBufferUsageFlags(createInfo.usage), buffers[i], nullptr, vkMemoryPropertyFlags, getVKAllocationCreateFlags(createInfo.memoryAllocationFlags));
}

Buffer::~Buffer() {
    for (uint8_t i = 0; i < _frameCount; i++)
        vmaDestroyBuffer(g_vulkan_device->allocator(), buffers[i], allocations[i]);
}

void Buffer::copyDataTo(void* data, size_t aSize) {
    if (aSize == 0)
        aSize = _size;
    uint8_t index = g_vulkan_swapChain->crntFrame() % _frameCount;

    void* mappedData;
    vmaMapMemory(g_vulkan_device->allocator(), allocations[index], &mappedData);
    memcpy(mappedData, data, aSize);
    vmaUnmapMemory(g_vulkan_device->allocator(), allocations[index]);
}

BufferDescriptorInfo Buffer::descriptorInfo(uint32_t binding, DescriptorType descriptorType/*VkDeviceSize size, VkDeviceSize offset*/) {
	BufferDescriptorInfo info;
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

} //namespace vulkan

} //namespace lv
