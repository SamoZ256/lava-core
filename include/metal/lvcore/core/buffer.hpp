#ifndef LV_METAL_BUFFER_H
#define LV_METAL_BUFFER_H

#include <vector>

#include "lvcore/core/common.hpp"

#include "common.hpp"

namespace lv {

struct Metal_BufferDescriptorInfo {
    std::vector<id /*MTLBuffer*/> buffers;
    uint32_t binding;
    DescriptorType descriptorType;
};

struct Metal_BufferCreateInfo {
    uint8_t frameCount = 0;
    size_t size;
    BufferUsageFlags usage = BufferUsageFlags::None;
    MemoryType memoryType = MemoryType::Private;
    MemoryAllocationCreateFlags memoryAllocationFlags = MemoryAllocationCreateFlags::None;
};

class Metal_Buffer {
private:
    uint8_t _frameCount;

    std::vector<id /*MTLBuffer*/> buffers;
    size_t _size;

public:
    Metal_Buffer(Metal_BufferCreateInfo createInfo);

    ~Metal_Buffer();

    void copyDataTo(void* data, size_t aSize = 0, uint32_t offset = 0);

    Metal_BufferDescriptorInfo descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::UniformBuffer);

    static void copyBufferToBuffer(id /*MTLBlitCommandEncoder*/ encoder, id /*MTLBuffer*/ srcBuffer, id /*MTLBuffer*/ dstBuffer, uint32_t size);

    static void copyBufferToImage(id /*MTLBlitCommandEncoder*/ encoder, id /*MTLBuffer*/ srcBuffer, id /*MTLTexture*/ dstImage, uint32_t width, uint32_t height, uint16_t bytesPerPixel = 4, uint8_t arrayLayer = 0, uint8_t mipLayer = 0);

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline id /*MTLBuffer*/ buffer(uint8_t index) { return buffers[index]; }

    inline size_t size() { return _size; }
};

} //namespace lv

#endif
