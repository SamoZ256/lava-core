#ifndef LV_VULKAN_IMAGE_H
#define LV_VULKAN_IMAGE_H

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include "lvcore/core/core.hpp"

#include "common.hpp"

#include "enums.hpp"

#include "buffer_helper.hpp"
#include "image_helper.hpp"

namespace lv {

class Vulkan_CommandBuffer;
class Vulkan_Image;

struct Vulkan_ImageDescriptorInfo {
    std::vector<VkDescriptorImageInfo> infos;
    uint32_t binding;
    VkDescriptorType descriptorType;
};

struct Vulkan_ImageCreateInfo {
    uint8_t frameCount = 0;
    Format format;
    uint16_t width;
    uint16_t height;
    uint16_t layerCount = 1;
    uint16_t mipCount = 1;
    LvImageViewType imageType = LV_IMAGE_VIEW_TYPE_2D;
    LvImageUsageFlags usage = 0;
    LvImageAspectFlags aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    LvMemoryType memoryType = LV_MEMORY_TYPE_PRIVATE;
    LvMemoryAllocationCreateFlags memoryAllocationFlags = 0;
};

struct Vulkan_ImageLoadInfo {
    const char* filename;
    bool isSRGB = false;
    bool generateMipmaps = false;
};

struct Vulkan_ImageViewCreateInfo {
    Vulkan_Image* image;
    LvImageViewType viewType;
    uint16_t baseLayer;
    uint16_t layerCount;
    uint16_t baseMip;
    uint16_t mipCount;
};

class Vulkan_Image {
private:
    uint8_t _frameCount;

    std::vector<VkImage> images;
    std::vector<VmaAllocation> allocations;
    std::vector<VkImageView> imageViews;

    Format _format;

    uint16_t _width, _height;

    uint16_t _baseLayer = 0;
    uint16_t _layerCount = 1;
    uint16_t _baseMip = 0;
    uint16_t _mipCount = 1;

    LvImageAspectFlags _aspectMask;

    bool _isOriginal = true;

public:
    Vulkan_Image(Vulkan_ImageCreateInfo createInfo);

    Vulkan_Image(Vulkan_ImageLoadInfo loadInfo, Vulkan_CommandBuffer* commandBuffer);

    Vulkan_Image(Vulkan_ImageViewCreateInfo viewCreateInfo);

    ~Vulkan_Image();

    void create(Vulkan_ImageCreateInfo& createInfo);

    void _createImageView(LvImageViewType viewType, LvImageAspectFlags aspectMask);

    Vulkan_ImageDescriptorInfo descriptorInfo(uint32_t binding, VkDescriptorType descriptorType = LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VkImageLayout imageLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, int8_t frameOffset = 0);
    
    Vulkan_Image* newImageView(LvImageViewType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount);

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline VkImage image(uint8_t index) { return images[index]; }

    inline VkImageView imageView(uint8_t index) { return imageViews[index]; }

    inline uint16_t width() { return _width; }

    inline uint16_t height() { return _height; }

    inline lv::Format format() { return _format; }

    inline uint16_t baseLayer() { return _baseLayer; }

    inline uint16_t layerCount() { return _layerCount; }

    inline uint16_t baseMip() { return _baseMip; }

    inline uint16_t mipCount() { return _mipCount; }

    inline LvImageAspectFlags aspectMask() { return _aspectMask; }

    inline bool isOriginal() { return _isOriginal; }

    inline VkImage* _imagesData() { return images.data(); }

    //Setters
    inline void _setFrameCount(uint8_t frameCount) {
        _frameCount = frameCount;
        images.resize(_frameCount);
        imageViews.resize(_frameCount);
    }

    inline void _setImage(VkImage image, uint8_t index) { images[index] = image; }

    inline void _setFormat(lv::Format format) { _format = format; }

    inline void _setWidth(uint16_t width) { _width = width; }

    inline void _setHeight(uint16_t height) { _height = height; }

    inline void _setBaseLayer(uint16_t baseLayer) { _baseLayer = baseLayer; }

    inline void _setLayerCount(uint16_t layerCount) { _layerCount = layerCount; }

    inline void _setBaseMip(uint16_t baseMip) { _baseMip = baseMip; }

    inline void _setMipCount(uint16_t mipCount) { _mipCount = mipCount; }

    inline void _setAspectMask(LvImageAspectFlags aspectMask) { _aspectMask = aspectMask; }
};

} //namespace lv

#endif
