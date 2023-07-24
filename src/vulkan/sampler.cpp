#include "vulkan/lvcore/sampler.hpp"

#include "vulkan/lvcore/core.hpp"

#include "vulkan/lvcore/device.hpp"

namespace lv {

namespace vulkan {

Sampler::Sampler(internal::SamplerCreateInfo createInfo) {
    VkFilter vkFilter;
    GET_VK_FILTER(createInfo.filter, vkFilter);
    VkCompareOp vkCompareOp;
    GET_VK_COMPARE_OP(createInfo.compareOp, vkCompareOp);

    ImageHelper::createImageSampler(_sampler, vkFilter, (VkSamplerAddressMode)createInfo.addressMode, (createInfo.compareEnable ? vkCompareOp : VK_COMPARE_OP_MAX_ENUM), createInfo.minLod, createInfo.maxLod);
}

Sampler::~Sampler() {
    vkDestroySampler(g_vulkan_device->device(), _sampler, nullptr);
}

internal::ImageDescriptorInfo* Sampler::descriptorInfo(internal::Image* image, uint32_t binding, ImageLayout imageLayout, int8_t frameOffset) {
    CAST_FROM_INTERNAL(image, Image);

    VkImageLayout vkImageLayout;
    GET_VK_IMAGE_LAYOUT(imageLayout, vkImageLayout);

    ImageDescriptorInfo* info = new ImageDescriptorInfo;
    info->infos.resize(image_->frameCount());
    for (uint8_t i = 0; i < info->infos.size(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += image_->frameCount();
        else if (index >= image_->frameCount()) index -= image_->frameCount();
        info->infos[i].imageLayout = vkImageLayout;
        info->infos[i].imageView = image_->imageView(index);
        info->infos[i].sampler = _sampler;
    }
    info->binding = binding;
    info->descriptorType = DescriptorType::CombinedImageSampler;

    return info;
}

} //namespace vulkan

} //namespace lv
