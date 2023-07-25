#ifndef LV_VULKAN_COMMAND_BUFFER_H
#define LV_VULKAN_COMMAND_BUFFER_H

#include "lvcore/internal/command_buffer.hpp"

#include "semaphore.hpp"
#include "framebuffer.hpp"
#include "buffer.hpp"
#include "descriptor_set.hpp"
#include "compute_pipeline.hpp"
#include "viewport.hpp"
#include "graphics_pipeline.hpp"

namespace lv {

namespace vulkan {

struct BufferAllocation {
    VkBuffer buffer;
    VmaAllocation allocation;
};

class CommandBuffer : public internal::CommandBuffer {
private:
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkFence> fences;

    //Active
    bool renderPassBound = false;
    VkPipelineBindPoint pipelineBindPoint;
    PipelineLayout* activePipelineLayout;
    Framebuffer* activeFramebuffer;
    std::vector<BufferAllocation> stagingBufferAllocationsToDestroy;

public:
    CommandBuffer(internal::CommandBufferCreateInfo createInfo);

    ~CommandBuffer() override;

    void beginRecording(CommandBufferUsageFlags usage = CommandBufferUsageFlags::None) override;

    void endRecording() override;

    void submit(internal::Semaphore* waitSemaphore = nullptr, internal::Semaphore* signalSemaphore = nullptr) override;

    void beginRenderCommands(internal::Framebuffer* framebuffer) override;

    void beginComputeCommands() override;

    void beginBlitCommands() override;

    void endCommands() override;

    void waitUntilCompleted() override;

    VkCommandBuffer _getActiveCommandBuffer();

    //Commands

    //Render
    void cmdBindVertexBuffer(internal::Buffer* buffer) override;

    void cmdDraw(uint32_t vertexCount, uint32_t instanceCount = 1) override;

    void cmdDrawIndexed(internal::Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount = 1) override;

    void cmdDrawPatches(uint32_t controlPointCount, uint32_t patchCount, uint32_t instanceCount = 1) override { LV_WARN_UNSUPPORTED_FUNCTION; }

    void cmdBindDescriptorSet(internal::DescriptorSet* descriptorSet) override;

    void cmdNextSubpass() override;

    void cmdBindGraphicsPipeline(internal::GraphicsPipeline* graphicsPipeline) override;

    void cmdPushConstants(void* data, uint16_t index) override;

    void cmdBindViewport(internal::Viewport* viewport) override;

    void cmdBindTessellationFactorBuffer(internal::Buffer* buffer) override { LV_WARN_UNSUPPORTED_FUNCTION; }

    void cmdBindTessellationFactorBufferForWriting(internal::Buffer* buffer) override { LV_WARN_UNSUPPORTED_FUNCTION; }

    void cmdEnableWireframeMode() override { LV_WARN_UNSUPPORTED_FUNCTION; }

    //Compute
    void cmdBindComputePipeline(internal::ComputePipeline* computePipeline) override;

    void cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ,
                              uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ) override;

    virtual void cmdDispatchTessellationControl(uint32_t patchCount) override { LV_WARN_UNSUPPORTED_FUNCTION; }

    //Blit
    void cmdStagingCopyDataToBuffer(internal::Buffer* buffer, void* data, size_t aSize = 0) override;

    void cmdStagingCopyDataToImage(internal::Image* image, void* data, uint8_t bytesPerPixel = 4) override;

    void cmdTransitionImageLayout(internal::Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout) override;

    void cmdGenerateMipmapsForImage(internal::Image* image, uint8_t aFrameCount = 0) override;

    void cmdCopyToImageFromImage(internal::Image* source, internal::Image* destination) override;

    void cmdBlitToImageFromImage(internal::Image* source, internal::Image* destination) override;

    //Other
    //TODO: use this function
    void cmdPresent() override {}

    //Getters
    inline VkCommandBuffer commandBuffer(uint8_t index) { return commandBuffers[index]; }
};

} //namespace vulkan

} //namespace lv

#endif
