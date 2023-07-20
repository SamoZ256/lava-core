#include "metal/lvcore/core/descriptor_set.hpp"

#include "metal/lvcore/core/swap_chain.hpp"

namespace lv {

Metal_DescriptorSet::Metal_DescriptorSet(Metal_DescriptorSetCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);
    _pipelineLayout = createInfo.pipelineLayout;
    _layoutIndex = createInfo.layoutIndex;

    for (auto& bufferBinding : createInfo.bufferBindings) {
        buffers.push_back(bufferBinding.buffers);
        bufferBindingIndices.push_back(bufferBinding.binding);
    }
    for (auto& imageBinding : createInfo.imageBindings) {
        if (imageBinding.descriptorType != DescriptorType::InputAttachment) {
            textures.push_back(imageBinding.images);
            textureBindingIndices.push_back(imageBinding.binding);
            if (imageBinding.descriptorType == DescriptorType::CombinedImageSampler) {
                samplers.push_back(imageBinding.sampler);
                samplerBindingIndices.push_back(imageBinding.binding);
            }
        }
    }
}

} //namespace lv
