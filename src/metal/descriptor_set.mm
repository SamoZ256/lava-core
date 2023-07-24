#include "metal/lvcore/descriptor_set.hpp"

#include "metal/lvcore/swap_chain.hpp"

namespace lv {

namespace metal {

DescriptorSet::DescriptorSet(internal::DescriptorSetCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);
    _pipelineLayout = (PipelineLayout*)createInfo.pipelineLayout;
    _layoutIndex = createInfo.layoutIndex;

    for (auto* bufferBinding : createInfo.bufferBindings) {
        CAST_FROM_INTERNAL(bufferBinding, BufferDescriptorInfo);

        buffers.push_back(bufferBinding_->buffers);
        bufferBindingIndices.push_back(bufferBinding_->binding);
        free(bufferBinding);
    }
    for (auto* imageBinding : createInfo.imageBindings) {
        CAST_FROM_INTERNAL(imageBinding, ImageDescriptorInfo);

        if (imageBinding_->descriptorType != DescriptorType::InputAttachment) {
            textures.push_back(imageBinding_->images);
            textureBindingIndices.push_back(imageBinding_->binding);
            if (imageBinding_->descriptorType == DescriptorType::CombinedImageSampler) {
                samplers.push_back(imageBinding_->sampler);
                samplerBindingIndices.push_back(imageBinding_->binding);
            }
        }
        free(imageBinding);
    }
}

} //namespace metal

} //namespace lv
