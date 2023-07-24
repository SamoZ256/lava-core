#ifndef LV_INTERNAL_COMMAND_BUFFER_H
#define LV_INTERNAL_COMMAND_BUFFER_H

#include "compute_pipeline.hpp"
#include "semaphore.hpp"
#include "framebuffer.hpp"
#include "viewport.hpp"
#include "descriptor_set.hpp"
#include "graphics_pipeline.hpp"

namespace lv {

namespace internal {

struct CommandBufferCreateInfo {
    uint8_t frameCount = 0;
    uint8_t threadIndex = 0;
    CommandBufferCreateFlags flags = CommandBufferCreateFlags::None;
};

class CommandBuffer {
protected:
    uint8_t frameCount;

    uint8_t threadIndex;

public:
    virtual ~CommandBuffer() {}

    virtual void beginRecording(CommandBufferUsageFlags usage = CommandBufferUsageFlags::None) = 0;

    virtual void endRecording() = 0;

    virtual void submit(Semaphore* waitSemaphore = nullptr, Semaphore* signalSemaphore = nullptr) = 0;

    virtual void beginRenderCommands(Framebuffer* framebuffer) = 0;

    virtual void beginComputeCommands() = 0;

    virtual void beginBlitCommands() = 0;

    virtual void endCommands() = 0;

    virtual void waitUntilCompleted() = 0;

    //Commands

    //Render
    virtual void cmdBindVertexBuffer(Buffer* buffer) = 0;

    virtual void cmdDraw(uint32_t vertexCount, uint32_t instanceCount = 1) = 0;

    virtual void cmdDrawIndexed(Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount = 1) = 0;

    //TODO: add option to specify index buffer?
    virtual void cmdDrawPatches(uint32_t controlPointCount, uint32_t patchCount, uint32_t instanceCount = 1) = 0; //Metal only

    virtual void cmdBindDescriptorSet(DescriptorSet* descriptorSet) = 0;

    virtual void cmdNextSubpass() = 0;

    virtual void cmdBindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) = 0;

    virtual void cmdPushConstants(void* data, uint16_t index) = 0;

    virtual void cmdBindViewport(Viewport* viewport) = 0;

    virtual void cmdBindTessellationFactorBuffer(Buffer* buffer) = 0; //Metal only

    virtual void cmdBindTessellationFactorBufferForWriting(Buffer* buffer) = 0; //Metal only

    virtual void cmdEnableWireframeMode() = 0; //Debug

    //Compute
    virtual void cmdBindComputePipeline(ComputePipeline* computePipeline) = 0;

    virtual void cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ,
                              uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ) = 0;

    virtual void cmdDispatchTessellationControl(uint32_t patchCount) = 0; //Metal only

    //Blit
    virtual void cmdStagingCopyDataToBuffer(Buffer* buffer, void* data, size_t aSize = 0) = 0;

    virtual void cmdStagingCopyDataToImage(Image* image, void* data, uint8_t bytesPerPixel = 4) = 0;

    virtual void cmdTransitionImageLayout(Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout) = 0;

    virtual void cmdGenerateMipmapsForImage(Image* image, uint8_t aFrameCount = 0) = 0;

    virtual void cmdCopyToImageFromImage(Image* source, Image* destination) = 0;

    virtual void cmdBlitToImageFromImage(Image* source, Image* destination) = 0;

    //Other
    virtual void cmdPresent() = 0;
};

} //namespace internal

} //namespace lv

#endif
