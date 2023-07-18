#include "vulkan/lvcore/core/image.hpp"

#include "stb/stb_image.h"
#include "gli/gli.hpp"

#include "vulkan/lvcore/core/device.hpp"
#include "vulkan/lvcore/core/swap_chain.hpp"

namespace lv {

Vulkan_Image::Vulkan_Image(Vulkan_ImageCreateInfo createInfo) {
    create(createInfo);
}

Vulkan_Image::Vulkan_Image(Vulkan_ImageLoadInfo loadInfo, Vulkan_CommandBuffer* commandBuffer) {
    std::string strFilename = std::string(loadInfo.filename);
    std::replace(strFilename.begin(), strFilename.end(), '\\', '/');
    std::string extension = strFilename.substr(strFilename.find_last_of('.'));
    int width, height, nbChannels;
    void* data = (void*)stbi_load(strFilename.c_str(), &width, &height, &nbChannels, STBI_rgb_alpha);

    //TODO: query the format from the image properties instead
    LvFormat format;
    if (loadInfo.isSRGB)
        format = LV_FORMAT_R8G8B8A8_UNORM_SRGB;
    else
        format = LV_FORMAT_R8G8B8A8_UNORM;

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

        Vulkan_ImageCreateInfo createInfo{
            .frameCount = 1,
            .format = format,
            .width = uint16_t(width),
            .height = uint16_t(height),
            .mipCount = mipCount,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_TRANSFER_DST_BIT, //TODO: let the user set this
        };
        create(createInfo);

        commandBuffer->cmdStagingCopyDataToImage(this, data);

        if (extension != ".dds")
            stbi_image_free(data);

        //TODO: uncomment this
        //if (loadInfo.generateMipmaps)
        //    commandBuffer->cmdGenerateMipmapsForImage(this);
    } else {
        Vulkan_ImageCreateInfo createInfo{
            .frameCount = 1,
            .format = format,
            .width = 1,
            .height = 1,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT
        };
        create(createInfo);

        std::cout << "Failed to load image '" << strFilename << "'" << std::endl;
    }
}

Vulkan_Image::Vulkan_Image(Vulkan_ImageViewCreateInfo viewCreateInfo) {
    _frameCount = viewCreateInfo.image->frameCount();
    _isOriginal = false;
    _format = viewCreateInfo.image->format();
    _width = viewCreateInfo.image->width();
    _height = viewCreateInfo.image->height();

    _baseLayer = viewCreateInfo.baseLayer;
    _layerCount = viewCreateInfo.layerCount;
    _baseMip = viewCreateInfo.baseMip;
    _mipCount = viewCreateInfo.mipCount;

    if (viewCreateInfo.viewType == LV_IMAGE_VIEW_TYPE_CUBE || viewCreateInfo.viewType == LV_IMAGE_VIEW_TYPE_CUBE_ARRAY)
        _layerCount *= 6;

    images.resize(_frameCount);
    imageViews.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        images[i] = viewCreateInfo.image->image(i);
    }
    _createImageView(viewCreateInfo.viewType, viewCreateInfo.image->aspectMask());
}

Vulkan_Image::~Vulkan_Image() {
    for (uint8_t i = 0; i < _frameCount; i++) {
        if (_isOriginal)
            vmaDestroyImage(g_vulkan_device->allocator(), images[i], allocations[i]);
        vkDestroyImageView(g_vulkan_device->device(), imageViews[i], nullptr);
    }
}

void Vulkan_Image::create(Vulkan_ImageCreateInfo& createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

    _width = createInfo.width;
    _height = createInfo.height;
    _format = createInfo.format;
    _layerCount = createInfo.layerCount;
    _mipCount = createInfo.mipCount;

    _aspectMask = createInfo.aspectMask;
    
    if (createInfo.imageType == LV_IMAGE_VIEW_TYPE_CUBE || createInfo.imageType == LV_IMAGE_VIEW_TYPE_CUBE_ARRAY)
        _layerCount *= 6;
    
#ifdef __APPLE__ //TODO: check for support instead
    if (createInfo.memoryType == LV_MEMORY_TYPE_MEMORYLESS)
        createInfo.memoryType = LV_MEMORY_TYPE_PRIVATE;
#endif

    LvMemoryAllocationCreateFlags memoryAllocationFlags = createInfo.memoryAllocationFlags;
    if (createInfo.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT || createInfo.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        memoryAllocationFlags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    VkImageCreateFlags flags = 0;
    if (createInfo.imageType == VK_IMAGE_VIEW_TYPE_CUBE || createInfo.imageType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
        flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    
    images.resize(_frameCount);
    allocations.resize(_frameCount);
    imageViews.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        //Creating image
        allocations[i] = Vulkan_ImageHelper::createImage((uint16_t)_width, (uint16_t)_height, _format, VK_IMAGE_TILING_OPTIMAL, createInfo.usage, images[i], nullptr, createInfo.memoryType, _layerCount, _mipCount, memoryAllocationFlags, flags);
        Vulkan_ImageHelper::createImageView(imageViews[i], images[i], _format, _aspectMask, createInfo.imageType, 0, _layerCount, 0, _mipCount);
    }
}

void Vulkan_Image::_createImageView(LvImageViewType viewType, LvImageAspectFlags aspectMask) {
    for (uint8_t i = 0; i < _frameCount; i++) {
        Vulkan_ImageHelper::createImageView(imageViews[i], images[i], _format, aspectMask, viewType, _baseLayer, _layerCount, _baseMip, _mipCount);
    }
}

Vulkan_ImageDescriptorInfo Vulkan_Image::descriptorInfo(uint32_t binding, VkDescriptorType descriptorType, VkImageLayout imageLayout, int8_t frameOffset) {
    Vulkan_ImageDescriptorInfo info;
    info.infos.resize(imageViews.size());
    for (uint8_t i = 0; i < info.infos.size(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += _frameCount;
        else if (index >= _frameCount) index -= _frameCount;
        info.infos[i].imageLayout = imageLayout;
        info.infos[i].imageView = imageViews[index];
        info.infos[i].sampler = VK_NULL_HANDLE;
    }
    info.binding = binding;
    info.descriptorType = descriptorType;

    return info;
}
    
Vulkan_Image* Vulkan_Image::newImageView(LvImageViewType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount) {
    Vulkan_Image* newImage = new Vulkan_Image({
        .image = this,
        .viewType = viewType,
        .baseLayer = baseLayer,
        .layerCount = layerCount,
        .baseMip = baseMip,
        .mipCount = mipCount
    });

    return newImage;
}

} //namespace lv
