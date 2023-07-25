#ifndef LV_LVCORE_H
#define LV_LVCORE_H

#include "lvcore/vulkan/buffer.hpp"
#include "lvcore/vulkan/command_buffer.hpp"
#include "lvcore/vulkan/compute_pipeline.hpp"
#include "lvcore/vulkan/device.hpp"
#include "lvcore/vulkan/descriptor_set.hpp"
#include "lvcore/vulkan/framebuffer.hpp"
#include "lvcore/vulkan/graphics_pipeline.hpp"
#include "lvcore/vulkan/image.hpp"
#include "lvcore/vulkan/instance.hpp"
#include "lvcore/vulkan/pipeline_layout.hpp"
#include "lvcore/vulkan/render_pass.hpp"
#include "lvcore/vulkan/sampler.hpp"
#include "lvcore/vulkan/semaphore.hpp"
#include "lvcore/vulkan/shader_module.hpp"
#include "lvcore/vulkan/swap_chain.hpp"
#include "lvcore/vulkan/vertex_descriptor.hpp"
#include "lvcore/vulkan/viewport.hpp"

#ifdef __APPLE__
#include "lvcore/metal/buffer.hpp"
#include "lvcore/metal/command_buffer.hpp"
#include "lvcore/metal/compute_pipeline.hpp"
#include "lvcore/metal/device.hpp"
#include "lvcore/metal/descriptor_set.hpp"
#include "lvcore/metal/framebuffer.hpp"
#include "lvcore/metal/graphics_pipeline.hpp"
#include "lvcore/metal/image.hpp"
#include "lvcore/metal/instance.hpp"
#include "lvcore/metal/pipeline_layout.hpp"
#include "lvcore/metal/render_pass.hpp"
#include "lvcore/metal/sampler.hpp"
#include "lvcore/metal/semaphore.hpp"
#include "lvcore/metal/shader_module.hpp"
#include "lvcore/metal/swap_chain.hpp"
#include "lvcore/metal/vertex_descriptor.hpp"
#include "lvcore/metal/viewport.hpp"
#endif

namespace lv {

typedef internal::BufferCreateInfo BufferCreateInfo;
typedef internal::Buffer Buffer;

typedef internal::CommandBufferCreateInfo CommandBufferCreateInfo;
typedef internal::CommandBuffer CommandBuffer;

typedef internal::ComputePipelineCreateInfo ComputePipelineCreateInfo;
typedef internal::ComputePipeline ComputePipeline;

typedef internal::DeviceCreateInfo DeviceCreateInfo;

typedef internal::DescriptorSetCreateInfo DescriptorSetCreateInfo;
typedef internal::DescriptorSet DescriptorSet;

typedef internal::FramebufferCreateInfo FramebufferCreateInfo;
typedef internal::Framebuffer Framebuffer;

typedef internal::GraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;
typedef internal::GraphicsPipeline GraphicsPipeline;

typedef internal::ImageCreateInfo ImageCreateInfo;
typedef internal::ImageLoadInfo ImageLoadInfo;
typedef internal::ImageViewCreateInfo ImageViewCreateInfo;
typedef internal::Image Image;

typedef internal::InstanceCreateInfo InstanceCreateInfo;

typedef internal::PipelineLayoutCreateInfo PipelineLayoutCreateInfo;
typedef internal::PipelineLayout PipelineLayout;

typedef internal::SubpassCreateInfo SubpassCreateInfo;
typedef internal::RenderPassCreateInfo RenderPassCreateInfo;
typedef internal::Subpass Subpass;
typedef internal::RenderPass RenderPass;

typedef internal::SamplerCreateInfo SamplerCreateInfo;
typedef internal::Sampler Sampler;

typedef internal::ShaderModuleCreateInfo ShaderModuleCreateInfo;
typedef internal::ShaderModule ShaderModule;

typedef internal::SwapChainCreateInfo SwapChainCreateInfo;
typedef internal::SwapChain SwapChain;

typedef internal::VertexDescriptorBinding VertexDescriptorBinding;
typedef internal::VertexDescriptorCreateInfo VertexDescriptorCreateInfo;
typedef internal::VertexDescriptor VertexDescriptor;

class Instance;

class Device {
private:
    internal::Device* handle;

    Instance* instance;

public:
    Device(DeviceCreateInfo createInfo, Instance* aInstance);

    ~Device() {
        delete handle;
    }

    void waitIdle() {
        handle->waitIdle();
    }

    //Create
    Buffer* createBuffer(BufferCreateInfo createInfo);

    CommandBuffer* createCommandBuffer(CommandBufferCreateInfo createInfo);

    ComputePipeline* createComputePipeline(ComputePipelineCreateInfo createInfo);

    DescriptorSet* createDescriptorSet(DescriptorSetCreateInfo createInfo);

    Framebuffer* createFramebuffer(FramebufferCreateInfo createInfo);

    GraphicsPipeline* createGraphicsPipeline(GraphicsPipelineCreateInfo createInfo);

    Image* createImage(ImageCreateInfo createInfo);

    Image* loadImage(ImageLoadInfo loadInfo, CommandBuffer* commandBuffer);

    Image* createImageView(ImageViewCreateInfo viewCreateInfo);

    PipelineLayout* createPipelineLayout(PipelineLayoutCreateInfo createInfo);

    Subpass* createSubpass(SubpassCreateInfo createInfo);

    RenderPass* createRenderPass(RenderPassCreateInfo createInfo);

    Sampler* createSampler(SamplerCreateInfo createInfo);

    ShaderModule* createShaderModule(ShaderModuleCreateInfo createInfo);

    SwapChain* createSwapChain(SwapChainCreateInfo createInfo);

    VertexDescriptor* createVertexDescriptor(VertexDescriptorCreateInfo createInfo);
};

class Instance {
protected:
    RenderAPI renderAPI;
    Bool validationEnable;

    internal::Instance* handle;

public:
    Instance(InstanceCreateInfo createInfo) : renderAPI(createInfo.renderAPI), validationEnable(createInfo.validationEnable) {
        switch (renderAPI) {
        case RenderAPI::Vulkan:
            handle = new vulkan::Instance(createInfo);
            break;
        case RenderAPI::Metal:
#ifdef __APPLE__
            handle = new metal::Instance(createInfo);
#else
            LV_ERROR_UNSUPPORTED_API("Metal");
#endif
            break;
        default:
            LV_INVALID_ARGUMENT("createInfo.renderAPI");
            break;
        }
    }

    ~Instance() {
        delete handle;
    }

    //Create
    Device* createDevice(DeviceCreateInfo createInfo) {
        return new Device(createInfo, this);
    }

    //Getters
    inline RenderAPI getRenderAPI() { return renderAPI; }

    inline Bool getValidationEnable() { return validationEnable; }
};

extern Instance* g_instance;

} //namespace lv

#endif
