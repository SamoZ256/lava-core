#ifndef LV_VULKAN_COMMAND_BUFFER_H
#define LV_VULKAN_COMMAND_BUFFER_H

#include <vector>

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

struct CommandBufferCreateInfo {
    uint8_t frameCount = 0;
    uint8_t threadIndex = 0;
    CommandBufferCreateFlags flags = CommandBufferCreateFlags::None;
};

class CommandBuffer {
private:
    uint8_t frameCount;

    uint8_t threadIndex;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkFence> fences;

    //Active
    bool renderPassBound = false;
    VkPipelineBindPoint pipelineBindPoint;
    PipelineLayout* activePipelineLayout;
    Framebuffer* activeFramebuffer;
    std::vector<BufferAllocation> stagingBufferAllocationsToDestroy;

public:
    CommandBuffer(CommandBufferCreateInfo createInfo);

    ~CommandBuffer();

    void beginRecording(VkCommandBufferUsageFlags usage = 0);

    void endRecording();

    void submit(Semaphore* waitSemaphore = nullptr, Semaphore* signalSemaphore = nullptr);

    void beginRenderCommands(Framebuffer* framebuffer);

    void beginComputeCommands();

    void beginBlitCommands();

    void endCommands();

    void waitUntilCompleted();

    VkCommandBuffer _getActiveCommandBuffer();

    //Commands

    //Render
    void cmdBindVertexBuffer(Buffer* buffer);

    void cmdDraw(uint32_t vertexCount, uint32_t instanceCount = 1);

    void cmdDrawIndexed(Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount = 1);

    void cmdBindDescriptorSet(DescriptorSet* descriptorSet);

    void cmdNextSubpass();

    void cmdBindGraphicsPipeline(GraphicsPipeline* graphicsPipeline);

    void cmdPushConstants(void* data, uint16_t index);

    void cmdBindViewport(Viewport* viewport);

    //Compute
    void cmdBindComputePipeline(ComputePipeline* computePipeline);

    void cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ,
                              uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ);

    //Blit
    void cmdStagingCopyDataToBuffer(Buffer* buffer, void* data, size_t aSize = 0);

    void cmdStagingCopyDataToImage(Image* image, void* data, uint8_t bytesPerPixel = 4);

    void cmdTransitionImageLayout(Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout);

    void cmdGenerateMipmapsForImage(Image* image, uint8_t aFrameCount = 0);

    void cmdCopyToImageFromImage(Image* source, Image* destination);

    void cmdBlitToImageFromImage(Image* source, Image* destination);

    //Getters
    inline VkCommandBuffer commandBuffer(uint8_t index) { return commandBuffers[index]; }
};

} //namespace vulkan

} //namespace lv

#endif
