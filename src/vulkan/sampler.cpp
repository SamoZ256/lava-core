#include "vulkan/lvcore/sampler.hpp"

#include "vulkan/lvcore/core.hpp"

#include "vulkan/lvcore/device.hpp"

namespace lv {

namespace vulkan {

Sampler::Sampler(SamplerCreateInfo createInfo) {
    VkFilter vkFilter;
    GET_VK_FILTER(createInfo.filter, vkFilter);
    VkCompareOp vkCompareOp;
    GET_VK_COMPARE_OP(createInfo.compareOp, vkCompareOp);

    ImageHelper::createImageSampler(_sampler, vkFilter, (VkSamplerAddressMode)createInfo.addressMode, (createInfo.compareEnable ? vkCompareOp : VK_COMPARE_OP_MAX_ENUM), createInfo.minLod, createInfo.maxLod);
}

Sampler::~Sampler() {
    vkDestroySampler(g_vulkan_device->device(), _sampler, nullptr);
}

ImageDescriptorInfo Sampler::descriptorInfo(Image* image, uint32_t binding, ImageLayout imageLayout, int8_t frameOffset) {
    VkImageLayout vkImageLayout;
    GET_VK_IMAGE_LAYOUT(imageLayout, vkImageLayout);

    ImageDescriptorInfo info;
    info.infos.resize(image->frameCount());
    for (uint8_t i = 0; i < info.infos.size(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += image->frameCount();
        else if (index >= image->frameCount()) index -= image->frameCount();
        info.infos[i].imageLayout = vkImageLayout;
        info.infos[i].imageView = image->imageView(index);
        info.infos[i].sampler = _sampler;
    }
    info.binding = binding;
    info.descriptorType = DescriptorType::CombinedImageSampler;

    return info;
}

} //namespace vulkan

} //namespace lv
