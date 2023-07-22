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

struct Vulkan_BufferAllocation {
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct Vulkan_CommandBufferCreateInfo {
    uint8_t frameCount = 0;
    uint8_t threadIndex = 0;
    CommandBufferCreateFlags flags = CommandBufferCreateFlags::None;
};

class Vulkan_CommandBuffer {
private:
    uint8_t frameCount;

    uint8_t threadIndex;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkFence> fences;

    //Active
    bool renderPassBound = false;
    VkPipelineBindPoint pipelineBindPoint;
    Vulkan_PipelineLayout* activePipelineLayout;
    Vulkan_Framebuffer* activeFramebuffer;
    std::vector<Vulkan_BufferAllocation> stagingBufferAllocationsToDestroy;

public:
    Vulkan_CommandBuffer(Vulkan_CommandBufferCreateInfo createInfo);

    ~Vulkan_CommandBuffer();

    void beginRecording(VkCommandBufferUsageFlags usage = 0);

    void endRecording();

    void submit(Vulkan_Semaphore* waitSemaphore = nullptr, Vulkan_Semaphore* signalSemaphore = nullptr);

    void beginRenderCommands(Vulkan_Framebuffer* framebuffer);

    void beginComputeCommands();

    void beginBlitCommands();

    void endCommands();

    void waitUntilCompleted();

    VkCommandBuffer _getActiveCommandBuffer();

    //Commands

    //Render
    void cmdBindVertexBuffer(Vulkan_Buffer* buffer);

    void cmdDraw(uint32_t vertexCount, uint32_t instanceCount = 1);

    void cmdDrawIndexed(Vulkan_Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount = 1);

    void cmdBindDescriptorSet(Vulkan_DescriptorSet* descriptorSet);

    void cmdNextSubpass();

    void cmdBindGraphicsPipeline(Vulkan_GraphicsPipeline* graphicsPipeline);

    void cmdPushConstants(void* data, uint16_t index);

    void cmdBindViewport(Vulkan_Viewport* viewport);

    //Compute
    void cmdBindComputePipeline(Vulkan_ComputePipeline* computePipeline);

    void cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ,
                              uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ);

    //Blit
    void cmdStagingCopyDataToBuffer(Vulkan_Buffer* buffer, void* data, size_t aSize = 0);

    void cmdStagingCopyDataToImage(Vulkan_Image* image, void* data, uint8_t bytesPerPixel = 4);

    void cmdTransitionImageLayout(Vulkan_Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout);

    void cmdGenerateMipmapsForImage(Vulkan_Image* image, uint8_t aFrameCount = 0);

    void cmdCopyToImageFromImage(Vulkan_Image* source, Vulkan_Image* destination);

    void cmdBlitToImageFromImage(Vulkan_Image* source, Vulkan_Image* destination);

    //Getters
    inline VkCommandBuffer commandBuffer(uint8_t index) { return commandBuffers[index]; }
};

} //namespace lv

#endif
