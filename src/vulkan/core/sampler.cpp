#include "vulkan/lvcore/core/sampler.hpp"

#include "vulkan/lvcore/core/device.hpp"

namespace lv {

Vulkan_Sampler::Vulkan_Sampler(Vulkan_SamplerCreateInfo createInfo) {
    Vulkan_ImageHelper::createImageSampler(_sampler, createInfo.filter, (VkSamplerAddressMode)createInfo.addressMode, (createInfo.compareEnable ? createInfo.compareOp : VK_COMPARE_OP_MAX_ENUM), createInfo.minLod, createInfo.maxLod);
}

Vulkan_Sampler::~Vulkan_Sampler() {
    vkDestroySampler(g_vulkan_device->device(), _sampler, nullptr);
}

Vulkan_ImageDescriptorInfo Vulkan_Sampler::descriptorInfo(Vulkan_Image* image, uint32_t binding, VkImageLayout imageLayout, int8_t frameOffset) {
    Vulkan_ImageDescriptorInfo info;
    info.infos.resize(image->frameCount());
    for (uint8_t i = 0; i < info.infos.size(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += image->frameCount();
        else if (index >= image->frameCount()) index -= image->frameCount();
        info.infos[i].imageLayout = imageLayout;
        info.infos[i].imageView = image->imageView(index);
        info.infos[i].sampler = _sampler;
    }
    info.binding = binding;
    info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    return info;
}

} //namespace lv
