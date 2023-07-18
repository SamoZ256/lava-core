#include "metal/lvcore/core/sampler.hpp"

#include <string>

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

Metal_Sampler::Metal_Sampler(Metal_SamplerCreateInfo createInfo) {
    MTLSamplerDescriptor* descriptor = [[MTLSamplerDescriptor alloc] init];
    descriptor.magFilter = (MTLSamplerMinMagFilter)createInfo.filter;
    descriptor.rAddressMode = (MTLSamplerAddressMode)createInfo.addressMode;
    descriptor.sAddressMode = (MTLSamplerAddressMode)createInfo.addressMode;
    descriptor.tAddressMode = (MTLSamplerAddressMode)createInfo.addressMode;
    if (createInfo.compareEnable)
        descriptor.compareFunction = (MTLCompareFunction)createInfo.compareOp;
    descriptor.lodMinClamp = createInfo.minLod;
    descriptor.lodMaxClamp = createInfo.maxLod;
    if (createInfo.maxLod > 0.0f) {
        descriptor.mipFilter = MTLSamplerMipFilterLinear;
        descriptor.minFilter = MTLSamplerMinMagFilterLinear;
        //std::cout << "Sampler lod: " << maxLod << std::endl;
    }

    _sampler = [g_metal_device->device() newSamplerStateWithDescriptor:descriptor];
}

Metal_Sampler::~Metal_Sampler() {
    [_sampler release];
}

Metal_ImageDescriptorInfo Metal_Sampler::descriptorInfo(Metal_Image* image, uint32_t binding, LvImageLayout imageLayout, int8_t frameOffset) {
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
    info.descriptorType = LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    return info;
}

} //namespace lv
