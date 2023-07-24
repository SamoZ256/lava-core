#include "metal/lvcore/buffer.hpp"

#import <Metal/Metal.h>

#include "metal/lvcore/core.hpp"

#include "metal/lvcore/device.hpp"
#include "metal/lvcore/swap_chain.hpp"

#include <iostream>

namespace lv {

namespace metal {

Buffer::Buffer(internal::BufferCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);
    _size = createInfo.size;

    MTLResourceOptions mtlResourceOptions;
    GET_MTL_RESOURCE_OPTIONS(createInfo.memoryType, mtlResourceOptions);

    buffers.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++)
        buffers[i] = [g_metal_device->device() newBufferWithLength:_size
                                                         options:mtlResourceOptions];
}

Buffer::~Buffer() {
    for (uint8_t i = 0; i < _frameCount; i++)
        [buffers[i] release];
}

void Buffer::copyDataTo(void* data, size_t aSize, uint32_t offset) {
    if (aSize == 0)
        aSize = _size;

    memcpy((char*)[buffers[g_metal_swapChain->crntFrame() % _frameCount] contents] + offset, data, aSize);
}

internal::BufferDescriptorInfo* Buffer::descriptorInfo(uint32_t binding, DescriptorType descriptorType) {
	BufferDescriptorInfo* info = new BufferDescriptorInfo();
	info->buffers.resize(_frameCount);
	for (uint8_t i = 0; i < _frameCount; i++) {
		info->buffers[i] = buffers[i];
	}
    info->binding = binding;
	info->descriptorType = descriptorType;

	return info;
}

void Buffer::copyBufferToBuffer(id encoder, id srcBuffer, id dstBuffer, uint32_t size) {
    [encoder copyFromBuffer:srcBuffer
                          sourceOffset:0
                              toBuffer:dstBuffer
                     destinationOffset:0
                                  size:size];
}

void Buffer::copyBufferToImage(id encoder, id srcBuffer, id dstImage, uint32_t width, uint32_t height, uint16_t bytesPerPixel, uint8_t arrayLayer, uint8_t mipLayer) {
    [encoder copyFromBuffer:srcBuffer
                          sourceOffset:0
                     sourceBytesPerRow:width * bytesPerPixel
                   sourceBytesPerImage:width * height * bytesPerPixel
                            sourceSize:MTLSizeMake(width, height, 1)
                             toTexture:dstImage
                      destinationSlice:arrayLayer
                      destinationLevel:mipLayer
                     destinationOrigin:MTLOriginMake(0, 0, 0)];
}

} //namespace metal

} //namespace lv
