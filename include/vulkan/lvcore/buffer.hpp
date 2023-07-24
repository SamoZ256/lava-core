#ifndef LV_VULKAN_BUFFER_H
#define LV_VULKAN_BUFFER_H

#include <vector>

#include "core.hpp"

namespace lv {

namespace vulkan {

struct BufferDescriptorInfo {
    std::vector<VkDescriptorBufferInfo> infos;
    uint32_t binding;
    DescriptorType descriptorType;
};

struct BufferCreateInfo {
    uint8_t frameCount = 0;
    size_t size;
    BufferUsageFlags usage = BufferUsageFlags::None;
    MemoryType memoryType = MemoryType::Private;
    MemoryAllocationCreateFlags memoryAllocationFlags = MemoryAllocationCreateFlags::None;
};

class Buffer {
private:
	uint8_t _frameCount = 0;
	
	std::vector<VkBuffer> buffers;
	std::vector<VmaAllocation> allocations;

	size_t _size;
	
public:
	Buffer(BufferCreateInfo createInfo);

    ~Buffer();

    void copyDataTo(void* data, size_t aSize = 0);

	BufferDescriptorInfo descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::UniformBuffer/*VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0*/);

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline VkBuffer buffer(uint8_t index) { return buffers[index]; }

    inline size_t size() { return _size; }
};

} //namespace vulkan

} //namespace lv

#endif
