#include "metal/lvcore/core/sampler.hpp"

#include <string>

#include "metal/lvcore/core/core.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

Metal_Sampler::Metal_Sampler(Metal_SamplerCreateInfo createInfo) {
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

Metal_Sampler::~Metal_Sampler() {
    [_sampler release];
}

Metal_ImageDescriptorInfo Metal_Sampler::descriptorInfo(Metal_Image* image, uint32_t binding, ImageLayout imageLayout, int8_t frameOffset) {
    Metal_ImageDescriptorInfo info;
    info.images.resize(image->frameCount());
    for (uint8_t i = 0; i < image->frameCount(); i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += image->frameCount();
        else if (index >= image->frameCount()) index -= image->frameCount();
        info.images[i] = image->image(index);
    }
    info.sampler = _sampler;
    info.binding = binding;
    info.descriptorType = DescriptorType::CombinedImageSampler;

    return info;
}

} //namespace lv
