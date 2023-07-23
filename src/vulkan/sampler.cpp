#include "vulkan/lvcore/core/sampler.hpp"

#include "vulkan/lvcore/core/core.hpp"

#include "vulkan/lvcore/core/device.hpp"

namespace lv {

Vulkan_Sampler::Vulkan_Sampler(Vulkan_SamplerCreateInfo createInfo) {
    VkFilter vkFilter;
    GET_VK_FILTER(createInfo.filter, vkFilter);
    VkCompareOp vkCompareOp;
    GET_VK_COMPARE_OP(createInfo.compareOp, vkCompareOp);

    Vulkan_ImageHelper::createImageSampler(_sampler, vkFilter, (VkSamplerAddressMode)createInfo.addressMode, (createInfo.compareEnable ? vkCompareOp : VK_COMPARE_OP_MAX_ENUM), createInfo.minLod, createInfo.maxLod);
}

Vulkan_Sampler::~Vulkan_Sampler() {
    vkDestroySampler(g_vulkan_device->device(), _sampler, nullptr);
}

Vulkan_ImageDescriptorInfo Vulkan_Sampler::descriptorInfo(Vulkan_Image* image, uint32_t binding, ImageLayout imageLayout, int8_t frameOffset) {
    VkImageLayout vkImageLayout;
    GET_VK_IMAGE_LAYOUT(imageLayout, vkImageLayout);

    Vulkan_ImageDescriptorInfo info;
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

} //namespace lv
