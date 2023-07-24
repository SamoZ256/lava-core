#include "vulkan/lvcore/image_helper.hpp"

#include "vulkan/lvcore/common.hpp"

#include "vulkan/lvcore/buffer_helper.hpp"
#include "vulkan/lvcore/device.hpp"

namespace lv {

namespace vulkan {

VmaAllocation ImageHelper::createImage(uint16_t width, uint16_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage/*, VmaMemoryUsage memoryUsage*/, VkImageType type, VkImage& image, VmaAllocationInfo* allocInfo, VkMemoryPropertyFlags properties, uint8_t layerCount, uint8_t mipCount, VmaAllocationCreateFlags allocationFlags, VkImageCreateFlags flags) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = type;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipCount;
    imageInfo.arrayLayers = layerCount;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = flags; // Optional

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
    allocCreateInfo.requiredFlags = properties;
    allocCreateInfo.flags = allocationFlags;
    
    VmaAllocation allocation;
    VK_CHECK_RESULT(vmaCreateImage(g_vulkan_device->allocator(), &imageInfo, &allocCreateInfo, &image, &allocation, allocInfo))

    return allocation;
}

void ImageHelper::transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, uint8_t layerCount, uint8_t mipCount) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    //Masks
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier.srcAccessMask = 0;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Unsupported old layout");
    }

    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Unsupported new layout");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

void ImageHelper::createImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageViewType viewType, uint8_t baseLayer, uint8_t layerCount, uint8_t baseMip, uint8_t mipCount) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange = {};
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = baseMip;
    viewInfo.subresourceRange.levelCount = mipCount;
    viewInfo.subresourceRange.baseArrayLayer = baseLayer;
    viewInfo.subresourceRange.layerCount = layerCount;

    VK_CHECK_RESULT(vkCreateImageView(g_vulkan_device->device(), &viewInfo, nullptr, &imageView))
}

void ImageHelper::createImageSampler(VkSampler& sampler, VkFilter filter, VkSamplerAddressMode addressMode, VkCompareOp compareOp, float minLod, float maxLod) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = (maxLod > 0.0f ? VK_FILTER_LINEAR : VK_FILTER_NEAREST);
    samplerInfo.addressModeU = addressMode;
    samplerInfo.addressModeV = addressMode;
    samplerInfo.addressModeW = addressMode;

    //Anastropic filtering
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(g_vulkan_device->physicalDevice(), &properties);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.compareEnable = (compareOp != VK_COMPARE_OP_MAX_ENUM);
    samplerInfo.compareOp = compareOp;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = minLod;
    samplerInfo.maxLod = maxLod;

    VK_CHECK_RESULT(vkCreateSampler(g_vulkan_device->device(), &samplerInfo, nullptr, &sampler))
}

} //namespace vulkan

} //namespace lv
