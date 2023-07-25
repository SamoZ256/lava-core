#include "lvcore/lvcore.hpp"

#ifdef __APPLE__

#define RENDER_API_SWICTH_WITH_INFO_NAMED(type, infoName) \
switch (instance->getRenderAPI()) { \
case RenderAPI::Vulkan: \
    return new vulkan::type(infoName); \
case RenderAPI::Metal: \
    return new metal::type(infoName); \
default: \
    LV_INVALID_ARGUMENT("instance::renderAPI"); \
    return nullptr; \
}

#define RENDER_API_SWICTH(type) RENDER_API_SWICTH_WITH_INFO_NAMED(type, createInfo)

#else

#define RENDER_API_SWICTH_WITH_INFO_NAMED(type, infoName) \
switch (instance->getRenderAPI()) { \
case RenderAPI::Vulkan: \
    return new vulkan::type(infoName); \
default: \
    LV_INVALID_ARGUMENT("instance::renderAPI"); \
    return nullptr; \
}

#define RENDER_API_SWICTH(type) RENDER_API_SWICTH_WITH_INFO_NAMED(type, createInfo)

#endif

namespace lv {

Device::Device(DeviceCreateInfo createInfo, Instance* aInstance) : instance(aInstance) {
    switch (instance->getRenderAPI()) {
    case RenderAPI::Vulkan:
        handle = new vulkan::Device(createInfo);
        break;
    case RenderAPI::Metal:
#ifdef __APPLE__
        handle = new metal::Device(createInfo);
#else
        LV_ERROR_UNSUPPORTED_API("Metal");
#endif
        break;
    default:
        LV_INVALID_ARGUMENT("instance::renderAPI");
        break;
    }
}

Buffer* Device::createBuffer(BufferCreateInfo createInfo) {
    RENDER_API_SWICTH(Buffer);
}

CommandBuffer* Device::createCommandBuffer(CommandBufferCreateInfo createInfo) {
    RENDER_API_SWICTH(CommandBuffer);
}

ComputePipeline* Device::createComputePipeline(ComputePipelineCreateInfo createInfo) {
    RENDER_API_SWICTH(ComputePipeline);
}

DescriptorSet* Device::createDescriptorSet(DescriptorSetCreateInfo createInfo) {
    RENDER_API_SWICTH(DescriptorSet);
}

Framebuffer* Device::createFramebuffer(FramebufferCreateInfo createInfo) {
    RENDER_API_SWICTH(Framebuffer);
}

GraphicsPipeline* Device::createGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) {
    RENDER_API_SWICTH(GraphicsPipeline);
}

Image* Device::createImage(ImageCreateInfo createInfo) {
    RENDER_API_SWICTH(Image);
}

Image* Device::loadImage(ImageLoadInfo loadInfo, CommandBuffer* commandBuffer) {
    switch (instance->getRenderAPI()) {
    case RenderAPI::Vulkan:
        return new vulkan::Image(loadInfo, (vulkan::CommandBuffer*)commandBuffer);
#ifdef __APPLE__
    case RenderAPI::Metal:
        return new metal::Image(loadInfo, (metal::CommandBuffer*)commandBuffer);
#endif
    default:
        LV_INVALID_ARGUMENT("instance::renderAPI");
        return nullptr;
    }
}

Image* Device::createImageView(ImageViewCreateInfo viewCreateInfo) {
    RENDER_API_SWICTH_WITH_INFO_NAMED(Image, viewCreateInfo);
}

PipelineLayout* Device::createPipelineLayout(PipelineLayoutCreateInfo createInfo) {
    RENDER_API_SWICTH(PipelineLayout);
}

Subpass* Device::createSubpass(SubpassCreateInfo createInfo) {
    RENDER_API_SWICTH(Subpass);
}

RenderPass* Device::createRenderPass(RenderPassCreateInfo createInfo) {
    RENDER_API_SWICTH(RenderPass);
}

Sampler* Device::createSampler(SamplerCreateInfo createInfo) {
    RENDER_API_SWICTH(Sampler);
}

ShaderModule* Device::createShaderModule(ShaderModuleCreateInfo createInfo) {
    RENDER_API_SWICTH(ShaderModule);
}

SwapChain* Device::createSwapChain(SwapChainCreateInfo createInfo) {
    RENDER_API_SWICTH(SwapChain);
}

VertexDescriptor* Device::createVertexDescriptor(VertexDescriptorCreateInfo createInfo) {
    RENDER_API_SWICTH(VertexDescriptor);
}

} //namespace lv
