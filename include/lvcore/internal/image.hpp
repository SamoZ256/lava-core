#ifndef LV_INTERNAL_IMAGE_H
#define LV_INTERNAL_IMAGE_H

#include "common.hpp"

namespace lv {

namespace internal {

class Image;

struct ImageDescriptorInfo {

};

struct ImageCreateInfo {
    uint8_t frameCount = 0;
    Format format;
    uint16_t width;
    uint16_t height;
    uint16_t layerCount = 1;
    uint16_t mipCount = 1;
    ImageType imageType = ImageType::_2D;
    ImageUsageFlags usage = ImageUsageFlags::None;
    ImageAspectFlags aspect = ImageAspectFlags::Color;
    MemoryType memoryType = MemoryType::Private;
    MemoryAllocationCreateFlags memoryAllocationFlags = MemoryAllocationCreateFlags::None;
};

struct ImageLoadInfo {
    const char* filename;
    bool isSRGB = false;
    bool generateMipmaps = false;
};

struct ImageViewCreateInfo {
    Image* image;
    ImageType viewType;
    uint16_t baseLayer;
    uint16_t layerCount;
    uint16_t baseMip;
    uint16_t mipCount;
};

class Image {
protected:
    uint8_t _frameCount;

    Format _format;

    uint16_t _width, _height;

    uint16_t _baseLayer = 0;
    uint16_t _layerCount = 1;
    uint16_t _baseMip = 0;
    uint16_t _mipCount = 1;

    uint8_t _layersPerLayer = 1;
    Bool _isOriginal = True;
    
public:
    virtual ~Image() {}

    virtual ImageDescriptorInfo* descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::SampledImage, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0) = 0;
    
    virtual Image* newImageView(ImageType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount) = 0;

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline uint16_t width() { return _width; }

    inline uint16_t height() { return _height; }

    inline Format format() { return _format; }

    inline uint16_t baseLayer() { return _baseLayer; }

    inline uint16_t layerCount() { return _layerCount; }

    inline uint16_t baseMip() { return _baseMip; }

    inline uint16_t mipCount() { return _mipCount; }

    inline uint8_t layersPerLayer() { return _layersPerLayer; }

    inline Bool isOriginal() { return _isOriginal; }
};

} //namespace internal

} //namespace lv

#endif
