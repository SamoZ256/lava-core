#ifndef LV_METAL_IMAGE_H
#define LV_METAL_IMAGE_H

#include <vector>

#include "lvcore/internal/common.hpp"
#include "lvcore/internal/image.hpp"

#include "common.hpp"
#include "compute_pipeline.hpp"

namespace lv {

namespace metal {

class CommandBuffer;

struct ImageDescriptorInfo : internal::ImageDescriptorInfo {
    std::vector<id /*MTLTexture*/> images;
    id /*MTLSamplerState*/ sampler;
    uint32_t binding;
    DescriptorType descriptorType;
};

class Image : public internal::Image {
private:
    std::vector<id /*MTLTexture*/> images;
    
public:
    static id /*MTLLibrary*/ blitComputeLibrary;
    static id /*MTLFunction*/ blitComputeFunction;
    static id /*MTLComputePipelineState*/ blitComputePipelineState;

    Image(internal::ImageCreateInfo createInfo);

    Image(internal::ImageLoadInfo loadInfo, CommandBuffer* commandBuffer);

    Image(internal::ImageViewCreateInfo viewCreateInfo);

    ~Image() override;

    internal::ImageDescriptorInfo* descriptorInfo(uint32_t binding, DescriptorType descriptorType = DescriptorType::SampledImage, ImageLayout imageLayout = ImageLayout::ShaderReadOnlyOptimal, int8_t frameOffset = 0) override;
    
    Image* newImageView(lv::ImageType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount) override;

    //Getters
    inline id /*MTLTexture*/ image(uint8_t index) { return images[index]; }

    //Setters
    inline void _setFrameCount(uint8_t frameCount) {
        _frameCount = frameCount;
        images.resize(_frameCount);
    }

    inline void _setImage(id /*MTLTexture*/ image, uint8_t index) { images[index] = image; }

    inline void _setFormat(Format format) { _format = format; }

    inline void _setWidth(uint16_t width) { _width = width; }

    inline void _setHeight(uint16_t height) { _height = height; }
};

} //namespace metal

} //namespace lv

#endif
