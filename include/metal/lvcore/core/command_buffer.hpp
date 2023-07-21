#ifndef LV_METAL_COMMAND_BUFFER_H
#define LV_METAL_COMMAND_BUFFER_H

#include <vector>

#include "common.hpp"

#include "semaphore.hpp"
#include "framebuffer.hpp"
#include "viewport.hpp"
#include "descriptor_set.hpp"
#include "graphics_pipeline.hpp"

namespace lv {

struct Metal_CommandBufferCreateInfo {
    uint8_t frameCount = 0;
    uint8_t threadIndex = 0;
    CommandBufferCreateFlags flags = CommandBufferCreateFlags::None;
};

class Metal_CommandBuffer {
private:
    uint8_t frameCount;

    uint8_t threadIndex;

    std::vector<id /*MTLCommandBuffer*/> commandBuffers;

    id /*MTLCommandEncoder*/ encoder;

    //Active
    bool isEncoding = false;
    void* /*MTLRenderPassDescriptor*/ activeRenderPass;
    Metal_PipelineLayout* activePipelineLayout;
    Metal_ShaderModule* activeShaderModules[3] = {nullptr};

public:
    Metal_CommandBuffer(Metal_CommandBufferCreateInfo createInfo);

    ~Metal_CommandBuffer() {}

    void beginRecording(CommandBufferUsageFlags usage = CommandBufferUsageFlags::None);

    void endRecording();

    void submit(Metal_Semaphore* waitSemaphore = nullptr, Metal_Semaphore* signalSemaphore = nullptr);

    void beginRenderCommands(Metal_Framebuffer* framebuffer);

    void beginComputeCommands();

    void beginBlitCommands();

    void endCommands();

    void waitUntilCompleted();

    id /*MTLCommandBuffer*/ _getActiveCommandBuffer();

    //Getters
    inline id /*MTLCommandEncoder*/ getEncoder() { return encoder; }

    //Commands

    //Render
    void cmdBindVertexBuffer(Metal_Buffer* buffer);

    void cmdDraw(uint32_t vertexCount, uint32_t instanceCount = 1);

    void cmdDrawIndexed(Metal_Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount = 1);

    //TODO: add option to specify index buffer?
    void cmdDrawPatches(uint32_t controlPointCount, uint32_t patchCount, uint32_t instanceCount = 1); //Metal only

    void cmdBindDescriptorSet(Metal_DescriptorSet* descriptorSet);

    void cmdNextSubpass() {}

    void cmdBindGraphicsPipeline(Metal_GraphicsPipeline* graphicsPipeline);

    void cmdPushConstants(void* data, uint16_t index);

    void cmdBindViewport(Metal_Viewport* viewport);

    void cmdBindTessellationFactorBuffer(Metal_Buffer* buffer); //Metal only

    void cmdBindTessellationFactorBufferForWriting(Metal_Buffer* buffer); //Metal only

    void cmdEnableWireframeMode(); //Debug

    //Compute
    void cmdBindComputePipeline(Metal_ComputePipeline* computePipeline);

    void cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ,
                              uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ);

    void cmdDispatchTessellationControl(uint32_t patchCount); //Metal only

    //Blit
    void cmdStagingCopyDataToBuffer(Metal_Buffer* buffer, void* data, size_t aSize = 0);

    void cmdStagingCopyDataToImage(Metal_Image* image, void* data, uint8_t bytesPerPixel = 4);

    void cmdTransitionImageLayout(Metal_Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout) {}

    void cmdGenerateMipmapsForImage(Metal_Image* image, uint8_t aFrameCount = 0);

    void cmdCopyToImageFromImage(Metal_Image* source, Metal_Image* destination);

    void cmdBlitToImageFromImage(Metal_Image* source, Metal_Image* destination);

    //Other
    void cmdPresent();
};

} //namespace lv

#endif
