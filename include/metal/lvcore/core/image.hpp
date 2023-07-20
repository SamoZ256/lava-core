#ifndef LV_METAL_IMAGE_H
#define LV_METAL_IMAGE_H

#include <vector>

#include "lvcore/core/common.hpp"

#include "common.hpp"

#include "enums.hpp"

#include "compute_pipeline.hpp"

namespace lv {

class Metal_CommandBuffer;
class Metal_Image;

struct Metal_ImageDescriptorInfo {
    std::vector<id /*MTLTexture*/> images;
    id /*MTLSamplerState*/ sampler;
    uint32_t binding;
    DescriptorType descriptorType;
};

struct Metal_ImageCreateInfo {
    uint8_t frameCount = 0;
    Format format;
    uint16_t width;
    uint16_t height;
    uint16_t layerCount = 1;
    uint16_t mipCount = 1;
    ImageType imageType = ImageType::_2D;
    ImageUsageFlags usage = ImageUsageFlags::None;
    ImageAspectFlags aspectMask = ImageAspectFlags::Color;
    MemoryType memoryType = MemoryType::Private;
    MemoryAllocationCreateFlags memoryAllocationFlags = MemoryAllocationCreateFlags::None;
};

struct Metal_ImageLoadInfo {
    const char* filename;
    bool isSRGB = false;
    bool generateMipmaps = false;
};

struct Metal_ImageViewCreateInfo {
    Metal_Image* image;
    ImageType viewType;
    uint16_t baseLayer;
    uint16_t layerCount;
    uint16_t baseMip;
    uint16_t mipCount;
};

class Metal_Image {
private:
    uint8_t _frameCount;

    std::vector<id /*MTLTexture*/> images;

    Format _format;

    uint16_t _width, _height;

    uint16_t _baseLayer = 0;
    uint16_t _layerCount = 1;
    uint16_t _baseMip = 0;
    uint16_t _mipCount = 1;

    uint8_t _layersPerLayer = 1;
    bool _isOriginal = true;
    
public:
    static id /*MTLLibrary*/ blitComputeLibrary;
    static id /*MTLFunction*/ blitComputeFunction;
    static id /*MTLComputePipelineState*/ blitComputePipelineState;

    Metal_Image(Metal_ImageCreateInfo createInfo);

    Metal_Image(Metal_ImageLoadInfo loadInfo, Metal_CommandBuffer* commandBuffer);

    Metal_Image(Metal_ImageViewCreateInfo viewCreateInfo);

    ~Metal_Image();

    Metal_ImageDescriptorInfo descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::SampledImage, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0);
    
    Metal_Image* newImageView(lv::ImageType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount);

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline id /*MTLTexture*/ image(uint8_t index) { return images[index]; }

    inline uint16_t width() { return _width; }

    inline uint16_t height() { return _height; }

    inline lv::Format format() { return _format; }

    inline uint16_t baseLayer() { return _baseLayer; }

    inline uint16_t layerCount() { return _layerCount; }

    inline uint16_t baseMip() { return _baseMip; }

    inline uint16_t mipCount() { return _mipCount; }

    inline uint8_t layersPerLayer() { return _layersPerLayer; }

    inline bool isOriginal() { return _isOriginal; }

    //Setters
    inline void _setFrameCount(uint8_t frameCount) {
        _frameCount = frameCount;
        images.resize(_frameCount);
    }

    inline void _setImage(id /*MTLTexture*/ image, uint8_t index) { images[index] = image; }

    inline void _setFormat(lv::Format format) { _format = format; }

    inline void _setWidth(uint16_t width) { _width = width; }

    inline void _setHeight(uint16_t height) { _height = height; }
};

} //namespace lv

#endif
