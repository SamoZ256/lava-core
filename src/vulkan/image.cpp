#include "vulkan/lvcore/image.hpp"

#include "stb/stb_image.h"
#include "gli/gli.hpp"

#include "vulkan/lvcore/core.hpp"

#include "vulkan/lvcore/device.hpp"
#include "vulkan/lvcore/swap_chain.hpp"

namespace lv {

namespace vulkan {

Image::Image(internal::ImageCreateInfo createInfo) {
    create(createInfo);
}

Image::Image(internal::ImageLoadInfo loadInfo, CommandBuffer* commandBuffer) {
    std::string strFilename = std::string(loadInfo.filename);
    std::replace(strFilename.begin(), strFilename.end(), '\\', '/');
    std::string extension = strFilename.substr(strFilename.find_last_of('.'));
    int width, height, nbChannels;
    void* data = (void*)stbi_load(strFilename.c_str(), &width, &height, &nbChannels, STBI_rgb_alpha);

    //TODO: query the format from the image properties instead
    Format format;
    if (loadInfo.isSRGB)
        format = Format::RGBA8Unorm_sRGB;
    else
        format = Format::RGBA8Unorm;

    if (!data && extension == ".dds") {
        gli::texture image = gli::load(loadInfo.filename);
        auto extent = image.extent();
        if (!image.empty() && extent.x > 0 && extent.y > 0) {
            width = extent.x;
            height = extent.y;
            uint8_t* srcData = (uint8_t*)image.data();
        } else {
            std::cout << "Failed to read dds file" << std::endl;
        }
    }

    if (data) {
        uint16_t mipCount = 1;
        //TODO: uncomment this
        //if (loadInfo.generateMipmaps)
        //    mipCount = std::max(std::max(ceil(log2(width)), ceil(log2(height))), 1.0);

        internal::ImageCreateInfo createInfo{
            .frameCount = 1,
            .format = format,
            .width = uint16_t(width),
            .height = uint16_t(height),
            .mipCount = mipCount,
            .usage = ImageUsageFlags::Sampled | ImageUsageFlags::TransferDestination, //TODO: let the user set this
        };
        create(createInfo);

        commandBuffer->cmdStagingCopyDataToImage(this, data);

        if (extension != ".dds")
            stbi_image_free(data);

        //TODO: uncomment this
        //if (loadInfo.generateMipmaps)
        //    commandBuffer->cmdGenerateMipmapsForImage(this);
    } else {
        internal::ImageCreateInfo createInfo{
            .frameCount = 1,
            .format = format,
            .width = 1,
            .height = 1,
            .usage = ImageUsageFlags::Sampled
        };
        create(createInfo);

        std::cout << "Failed to load image '" << strFilename << "'" << std::endl;
    }
}

Image::Image(internal::ImageViewCreateInfo viewCreateInfo) {
    CAST_FROM_INTERNAL_NAMED(viewCreateInfo.image, Image, image);

    _frameCount = image->frameCount();
    _isOriginal = False;
    _format = image->format();
    _width = image->width();
    _height = image->height();

    _baseLayer = viewCreateInfo.baseLayer;
    _layerCount = viewCreateInfo.layerCount;
    _baseMip = viewCreateInfo.baseMip;
    _mipCount = viewCreateInfo.mipCount;

    if (viewCreateInfo.viewType == ImageType::Cube || viewCreateInfo.viewType == ImageType::CubeArray)
        _layerCount *= 6;

    images.resize(_frameCount);
    imageViews.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        images[i] = image->image(i);
    }
    _createImageView(viewCreateInfo.viewType, image->aspect());
}

Image::~Image() {
    for (uint8_t i = 0; i < _frameCount; i++) {
        if (_isOriginal)
            vmaDestroyImage(g_vulkan_device->allocator(), images[i], allocations[i]);
        vkDestroyImageView(g_vulkan_device->device(), imageViews[i], nullptr);
    }
}

void Image::create(internal::ImageCreateInfo& createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

    _width = createInfo.width;
    _height = createInfo.height;
    _format = createInfo.format;
    _layerCount = createInfo.layerCount;
    _mipCount = createInfo.mipCount;
    _aspect = createInfo.aspect;

    VkFormat vkFormat;
    GET_VK_FORMAT(_format, vkFormat);
    VkImageType vkImageType;
    GET_VK_IMAGE_TYPE(createInfo.imageType, vkImageType);
    VkImageViewType vkImageViewType;
    GET_VK_IMAGE_VIEW_TYPE(createInfo.imageType, vkImageViewType);
    
    VkImageCreateFlags flags = 0;
    if (createInfo.imageType == ImageType::Cube || createInfo.imageType == ImageType::CubeArray) {
        _layerCount *= 6;
        flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }
    
    //TODO: create a local variable for memoryType
#ifdef __APPLE__ //TODO: check for support instead
    if (createInfo.memoryType == MemoryType::Memoryless) {
        createInfo.memoryType = MemoryType::Private;
        LV_WARN_UNSUPPORTED("MemoryType::Memoryless");
    }
#endif

    VkMemoryPropertyFlags vkMemoryPropertyFlags;
    GET_VK_MEMORY_PROPERTIES(createInfo.memoryType, vkMemoryPropertyFlags);

    MemoryAllocationCreateFlags memoryAllocationFlags = createInfo.memoryAllocationFlags;
    if (createInfo.usage & ImageUsageFlags::ColorAttachment || createInfo.usage & ImageUsageFlags::DepthStencilAttachment)
        memoryAllocationFlags |= MemoryAllocationCreateFlags::Dedicated;
    
    images.resize(_frameCount);
    allocations.resize(_frameCount);
    imageViews.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        //Creating image
        allocations[i] = ImageHelper::createImage((uint16_t)_width, (uint16_t)_height, vkFormat, VK_IMAGE_TILING_OPTIMAL, getVKImageUsageFlags(createInfo.usage), vkImageType, images[i], nullptr, vkMemoryPropertyFlags, _layerCount, _mipCount, getVKAllocationCreateFlags(memoryAllocationFlags), flags);
        ImageHelper::createImageView(imageViews[i], images[i], vkFormat, getVKImageAspectFlags(_aspect), vkImageViewType, 0, _layerCount, 0, _mipCount);
    }
}

void Image::_createImageView(ImageType viewType, ImageAspectFlags aspect) {
    VkFormat vkFormat;
    GET_VK_FORMAT(_format, vkFormat);
    VkImageViewType vkImageViewType;
    GET_VK_IMAGE_VIEW_TYPE(viewType, vkImageViewType);

    for (uint8_t i = 0; i < _frameCount; i++) {
        ImageHelper::createImageView(imageViews[i], images[i], vkFormat, getVKImageAspectFlags(aspect), vkImageViewType, _baseLayer, _layerCount, _baseMip, _mipCount);
    }
}

internal::ImageDescriptorInfo* Image::descriptorInfo(uint32_t binding, DescriptorType descriptorType, ImageLayout imageLayout, int8_t frameOffset) {
    VkImageLayout vkImageLayout;
    GET_VK_IMAGE_LAYOUT(imageLayout, vkImageLayout);

    ImageDescriptorInfo* info = new ImageDescriptorInfo;
    info->infos.resize(imageViews.size());
    for (uint8_t i = 0; i < imageViews.size(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += _frameCount;
        else if (index >= _frameCount) index -= _frameCount;
        info->infos[i].imageLayout = vkImageLayout;
        info->infos[i].imageView = imageViews[index];
        info->infos[i].sampler = VK_NULL_HANDLE;
    }
    info->binding = binding;
    info->descriptorType = descriptorType;

    return info;
}
    
Image* Image::newImageView(ImageType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount) {
    Image* newImage = new Image({
        .image = this,
        .viewType = viewType,
        .baseLayer = baseLayer,
        .layerCount = layerCount,
        .baseMip = baseMip,
        .mipCount = mipCount
    });

    return newImage;
}

} //namespace vulkan

} //namespace lv
