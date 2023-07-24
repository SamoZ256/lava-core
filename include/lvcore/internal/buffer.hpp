#ifndef LV_INTERNAL_BUFFER_H
#define LV_INTERNAL_BUFFER_H

#include "common.hpp"

namespace lv {

namespace internal {

struct BufferDescriptorInfo {
    
};

struct BufferCreateInfo {
    uint8_t frameCount = 0;
    size_t size;
    BufferUsageFlags usage = BufferUsageFlags::None;
    MemoryType memoryType = MemoryType::Private;
    MemoryAllocationCreateFlags memoryAllocationFlags = MemoryAllocationCreateFlags::None;
};

class Buffer {
protected:
    uint8_t _frameCount;

    size_t _size;

public:
    virtual ~Buffer() {}

    virtual void copyDataTo(void* data, size_t aSize = 0, uint32_t offset = 0) = 0;

    virtual BufferDescriptorInfo* descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::UniformBuffer) = 0;

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline size_t size() { return _size; }
};

} //namespace internal

} //namespace lv

#endif
