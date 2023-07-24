#ifndef LV_VULKAN_IMAGE_H
#define LV_VULKAN_IMAGE_H

#include "lvcore/internal/image.hpp"

#include "common.hpp"

#include "buffer_helper.hpp"
#include "image_helper.hpp"

namespace lv {

namespace vulkan {

class CommandBuffer;

struct ImageDescriptorInfo : internal::ImageDescriptorInfo {
    std::vector<VkDescriptorImageInfo> infos;
    uint32_t binding;
    DescriptorType descriptorType;
};

class Image : public internal::Image {
private:
    std::vector<VkImage> images;
    std::vector<VmaAllocation> allocations;
    std::vector<VkImageView> imageViews;

public:
    Image(internal::ImageCreateInfo createInfo);

    Image(internal::ImageLoadInfo loadInfo, CommandBuffer* commandBuffer);

    Image(internal::ImageViewCreateInfo viewCreateInfo);

    ~Image() override;

    void create(internal::ImageCreateInfo& createInfo);

    void _createImageView(ImageType viewType, ImageAspectFlags aspect);

    internal::ImageDescriptorInfo* descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::SampledImage, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0) override;
    
    Image* newImageView(ImageType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount) override;

    //Getters
    inline VkImage image(uint8_t index) { return images[index]; }

    inline VkImageView imageView(uint8_t index) { return imageViews[index]; }

    inline VkImage* _imagesData() { return images.data(); }

    //Setters
    inline void _setFrameCount(uint8_t frameCount) {
        _frameCount = frameCount;
        images.resize(_frameCount);
        imageViews.resize(_frameCount);
    }

    inline void _setImage(VkImage image, uint8_t index) { images[index] = image; }

    inline void _setFormat(Format format) { _format = format; }

    inline void _setWidth(uint16_t width) { _width = width; }

    inline void _setHeight(uint16_t height) { _height = height; }

    inline void _setBaseLayer(uint16_t baseLayer) { _baseLayer = baseLayer; }

    inline void _setLayerCount(uint16_t layerCount) { _layerCount = layerCount; }

    inline void _setBaseMip(uint16_t baseMip) { _baseMip = baseMip; }

    inline void _setMipCount(uint16_t mipCount) { _mipCount = mipCount; }

    inline void _setAspect(ImageAspectFlags aspect) { _aspect = aspect; }
};

} //namespace vulkan

} //namespace lv

#endif
