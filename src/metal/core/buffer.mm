#include "metal/lvcore/core/buffer.hpp"

#import <Metal/Metal.h>

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

#include <iostream>

namespace lv {

Metal_Buffer::Metal_Buffer(Metal_BufferCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);
    _size = createInfo.size;

    MTLResourceOptions options = 0;
    if (createInfo.memoryType == LV_MEMORY_TYPE_PRIVATE)
        options |= MTLResourceStorageModePrivate;
    else if (createInfo.memoryType == LV_MEMORY_TYPE_SHARED)
        options |= MTLResourceStorageModeShared;
    else if (createInfo.memoryType == LV_MEMORY_TYPE_MEMORYLESS)
        options |= MTLResourceStorageModeMemoryless;

    buffers.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++)
        buffers[i] = [g_metal_device->device() newBufferWithLength:_size
                                                         options:options];
}

Metal_Buffer::~Metal_Buffer() {
    for (uint8_t i = 0; i < _frameCount; i++)
        [buffers[i] release];
}

void Metal_Buffer::copyDataTo(void* data, size_t aSize, uint32_t offset) {
    if (aSize == 0)
        aSize = _size;

    memcpy((char*)[buffers[g_metal_swapChain->crntFrame() % _frameCount] contents] + offset, data, aSize);
}

Metal_BufferDescriptorInfo Metal_Buffer::descriptorInfo(uint32_t binding, LvDescriptorType descriptorType) {
	Metal_BufferDescriptorInfo info;
	info.buffers.resize(_frameCount);
	for (uint8_t i = 0; i < _frameCount; i++) {
		info.buffers[i] = buffers[i];
	}
    info.binding = binding;
	info.descriptorType = descriptorType;

	return info;
}

void Metal_Buffer::copyBufferToBuffer(id encoder, id srcBuffer, id dstBuffer, uint32_t size) {
    [encoder copyFromBuffer:srcBuffer
                          sourceOffset:0
                              toBuffer:dstBuffer
                     destinationOffset:0
                                  size:size];
}

void Metal_Buffer::copyBufferToImage(id encoder, id srcBuffer, id dstImage, uint32_t width, uint32_t height, uint16_t bytesPerPixel, uint8_t arrayLayer, uint8_t mipLayer) {
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

} //namespace lv
