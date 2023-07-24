#include "metal/lvcore/sampler.hpp"

#include <string>

#include "metal/lvcore/core.hpp"

#include "metal/lvcore/device.hpp"
#include "metal/lvcore/swap_chain.hpp"

namespace lv {

namespace metal {

Sampler::Sampler(internal::SamplerCreateInfo createInfo) {
    MTLSamplerMinMagFilter mtlSamplerMinMagFilter;
    GET_MTL_SAMPLER_MIN_MAG_FILTER(createInfo.filter, mtlSamplerMinMagFilter);
    MTLSamplerAddressMode mtlSamplerAddressMode;
    GET_MTL_SAMPLER_ADDRESS_MODE(createInfo.addressMode, mtlSamplerAddressMode);
    MTLCompareFunction mtlCompareFunction;
    GET_MTL_COMPARE_FUNCTION(createInfo.compareOp, mtlCompareFunction);

    MTLSamplerDescriptor* descriptor = [[MTLSamplerDescriptor alloc] init];
    descriptor.magFilter = mtlSamplerMinMagFilter;
    descriptor.rAddressMode = mtlSamplerAddressMode;
    descriptor.sAddressMode = mtlSamplerAddressMode;
    descriptor.tAddressMode = mtlSamplerAddressMode;
    if (createInfo.compareEnable)
        descriptor.compareFunction = mtlCompareFunction;
    descriptor.lodMinClamp = createInfo.minLod;
    descriptor.lodMaxClamp = createInfo.maxLod;
    if (createInfo.maxLod > 0.0f) {
        //TODO: let the user specify defferent filters
        descriptor.mipFilter = MTLSamplerMipFilterLinear;
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
    }

    _sampler = [g_metal_device->device() newSamplerStateWithDescriptor:descriptor];
}

Sampler::~Sampler() {
    [_sampler release];
}

internal::ImageDescriptorInfo* Sampler::descriptorInfo(internal::Image* image, uint32_t binding, ImageLayout imageLayout, int8_t frameOffset) {
    CAST_FROM_INTERNAL(image, Image);

    ImageDescriptorInfo* info = new ImageDescriptorInfo;
    info->images.resize(image_->frameCount());
    for (uint8_t i = 0; i < image_->frameCount(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += image_->frameCount();
        else if (index >= image_->frameCount()) index -= image_->frameCount();
        info->images[i] = image_->image(index);
    }
    info->sampler = _sampler;
    info->binding = binding;
    info->descriptorType = DescriptorType::CombinedImageSampler;

    return info;
}

} //namespace metal

} //namespace lv
