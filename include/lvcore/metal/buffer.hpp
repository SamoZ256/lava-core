#ifndef LV_METAL_BUFFER_H
#define LV_METAL_BUFFER_H

#include <vector>

#include "lvcore/internal/common.hpp"
#include "lvcore/internal/buffer.hpp"

#include "common.hpp"

namespace lv {

namespace metal {

struct BufferDescriptorInfo : internal::BufferDescriptorInfo {
    std::vector<id /*MTLBuffer*/> buffers;
    uint32_t binding;
    DescriptorType descriptorType;
};

class Buffer : public internal::Buffer {
private:
    std::vector<id /*MTLBuffer*/> buffers;

public:
    Buffer(internal::BufferCreateInfo createInfo);

    ~Buffer() override;

    void copyDataTo(void* data, size_t aSize = 0, uint32_t offset = 0) override;

    internal::BufferDescriptorInfo* descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::UniformBuffer) override;

    static void copyBufferToBuffer(id /*MTLBlitCommandEncoder*/ encoder, id /*MTLBuffer*/ srcBuffer, id /*MTLBuffer*/ dstBuffer, uint32_t size);

    static void copyBufferToImage(id /*MTLBlitCommandEncoder*/ encoder, id /*MTLBuffer*/ srcBuffer, id /*MTLTexture*/ dstImage, uint32_t width, uint32_t height, uint16_t bytesPerPixel = 4, uint8_t arrayLayer = 0, uint8_t mipLayer = 0);

    //Getters
    inline id /*MTLBuffer*/ buffer(uint8_t index) { return buffers[index]; }
};

} //namespace metal

} //namespace lv

#endif
