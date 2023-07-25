#ifndef LV_VULKAN_BUFFER_H
#define LV_VULKAN_BUFFER_H

#include "lvcore/internal/buffer.hpp"

#include "core.hpp"

namespace lv {

namespace vulkan {

struct BufferDescriptorInfo : internal::BufferDescriptorInfo {
    std::vector<VkDescriptorBufferInfo> infos;
    uint32_t binding;
    DescriptorType descriptorType;
};

class Buffer : public internal::Buffer {
private:
	std::vector<VkBuffer> buffers;
	std::vector<VmaAllocation> allocations;
	
public:
	Buffer(internal::BufferCreateInfo createInfo);

    ~Buffer() override;

    void copyDataTo(void* data, size_t aSize = 0, uint32_t offset = 0) override;

	internal::BufferDescriptorInfo* descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::UniformBuffer/*VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0*/) override;

    //Getters
    inline VkBuffer buffer(uint8_t index) { return buffers[index]; }
};

} //namespace vulkan

} //namespace lv

#endif
