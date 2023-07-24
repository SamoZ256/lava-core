#include "vulkan/lvcore/command_buffer.hpp"

#include "vulkan/lvcore/core.hpp"

#include "vulkan/lvcore/swap_chain.hpp"

namespace lv {

namespace vulkan {

#define _LV_CHECK_IF_ENCODING \
if (renderPassBound) { \
    vkCmdEndRenderPass(_getActiveCommandBuffer()); \
    renderPassBound = false; \
}

CommandBuffer::CommandBuffer(CommandBufferCreateInfo createInfo) {
    frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

    threadIndex = createInfo.threadIndex;

    VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = g_vulkan_device->commandPool(threadIndex);
	allocInfo.commandBufferCount = frameCount;

    commandBuffers.resize(frameCount);
	VK_CHECK_RESULT(vkAllocateCommandBuffers(g_vulkan_device->device(), &allocInfo, commandBuffers.data()));

    if (createInfo.flags & CommandBufferCreateFlags::CreateFenceToWaitUntilComplete) {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        fences.resize(frameCount);
        for (uint8_t i = 0; i < frameCount; i++)
            VK_CHECK_RESULT(vkCreateFence(g_vulkan_device->device(), &fenceInfo, nullptr, &fences[i]));
    }
}

CommandBuffer::~CommandBuffer() {
    vkFreeCommandBuffers(g_vulkan_device->device(), g_vulkan_device->commandPool(threadIndex), commandBuffers.size(), commandBuffers.data());
}

void CommandBuffer::beginRecording(VkCommandBufferUsageFlags usage) {
    uint8_t index = g_vulkan_swapChain->crntFrame() % frameCount;
    index = g_vulkan_swapChain->crntFrame();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usage;

    VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[index], &beginInfo));
}

void CommandBuffer::endRecording() {
    _LV_CHECK_IF_ENCODING;

    uint8_t index = g_vulkan_swapChain->crntFrame() % frameCount;
    index = g_vulkan_swapChain->crntFrame();
    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[index]));
}

void CommandBuffer::submit(Semaphore* waitSemaphore, Semaphore* signalSemaphore) {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[g_vulkan_swapChain->crntFrame() % frameCount];
    if (waitSemaphore != nullptr) {
        VkSemaphore semaphores[] = {waitSemaphore->semaphore(g_vulkan_swapChain->crntFrame())}; //TODO: make this image count for swap chain semaphores
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = semaphores;
    }
    if (signalSemaphore != nullptr) {
        VkSemaphore semaphores[] = {signalSemaphore->semaphore(g_vulkan_swapChain->crntFrame())};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = semaphores;
    }
    
    VkFence fence = VK_NULL_HANDLE;
    if (fences.size() > 0) {
        fence = fences[g_vulkan_swapChain->crntFrame()];
	    vkResetFences(g_vulkan_device->device(), 1, &fence);
    }
    VK_CHECK_RESULT(vkQueueSubmit(g_vulkan_device->graphicsQueue(), 1, &submitInfo, fence));

    //TODO: destroy staging buffers
    /*
    for (auto& bufferAllocation : stagingBufferAllocationsToDestroy) {
        vmaDestroyBuffer(g_vulkan_device->allocator(), bufferAllocation.buffer, bufferAllocation.allocation);
    }

    stagingBufferAllocationsToDestroy.clear();
    */
}

void CommandBuffer::beginRenderCommands(Framebuffer* framebuffer) {
    _LV_CHECK_IF_ENCODING;
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = framebuffer->renderPass()->renderPass();
    renderPassInfo.framebuffer = framebuffer->framebuffer(g_vulkan_swapChain->crntFrame()); //TODO: use image index in case of swap chain framebuffer
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VkExtent2D{framebuffer->width(), framebuffer->height()};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(framebuffer->clearValueCount());
    renderPassInfo.pClearValues = framebuffer->clearValuesData();

    vkCmdBeginRenderPass(_getActiveCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    activeFramebuffer = framebuffer;

    renderPassBound = true;
}

void CommandBuffer::beginComputeCommands() {
    _LV_CHECK_IF_ENCODING;
}

void CommandBuffer::beginBlitCommands() {
    _LV_CHECK_IF_ENCODING;
}

void CommandBuffer::endCommands() {
    _LV_CHECK_IF_ENCODING;
}

void CommandBuffer::waitUntilCompleted() {
    vkWaitForFences(g_vulkan_device->device(), 1, &fences[g_vulkan_swapChain->crntFrame()], VK_TRUE, UINT64_MAX);
}

VkCommandBuffer CommandBuffer::_getActiveCommandBuffer() {
    return commandBuffers[g_vulkan_swapChain->crntFrame() % frameCount];
}

//Commands

//Render
void CommandBuffer::cmdBindVertexBuffer(Buffer* buffer) {
    VkDeviceSize offsets[] = {0};
    VkBuffer buffers[] = {buffer->buffer(g_vulkan_swapChain->crntFrame() % buffer->frameCount())};
    vkCmdBindVertexBuffers(_getActiveCommandBuffer(), 0, 1, buffers, offsets);
}

void CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount) {
    vkCmdDraw(_getActiveCommandBuffer(), vertexCount, instanceCount, 0, 0);
}

void CommandBuffer::cmdDrawIndexed(Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount) {
    VkIndexType vkIndexType;
    GET_VK_INDEX_TYPE(indexType, vkIndexType);
    
    vkCmdBindIndexBuffer(_getActiveCommandBuffer(), indexBuffer->buffer(g_vulkan_swapChain->crntFrame() % indexBuffer->frameCount()), 0, vkIndexType);

    vkCmdDrawIndexed(_getActiveCommandBuffer(), indexCount, instanceCount, 0, 0, 0);
}

void CommandBuffer::cmdBindDescriptorSet(DescriptorSet* descriptorSet) {
    VkDescriptorSet descriptorSets[] = {descriptorSet->descriptorSet(g_vulkan_swapChain->crntFrame() % descriptorSet->frameCount())};
    vkCmdBindDescriptorSets(_getActiveCommandBuffer(), pipelineBindPoint, descriptorSet->pipelineLayout()->pipelineLayout(), descriptorSet->layoutIndex(), 1, descriptorSets, 0, nullptr);
}

void CommandBuffer::cmdNextSubpass() {
    vkCmdNextSubpass(_getActiveCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::cmdBindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) {
    vkCmdBindPipeline(_getActiveCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline());
    pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    activePipelineLayout = graphicsPipeline->pipelineLayout();
    //std::cout << "DEPENDENCY COUNT: " << (int)g_metal_swapChain->activeFramebuffer->renderPass->dependencies.size() << std::endl;
    
    //HACK: set the viewport of the currently active framebuffer
    VkViewport viewport{
        .x = 0,
        .y = (float)activeFramebuffer->height(),
        .width = (float)activeFramebuffer->width(),
        .height = (float)-activeFramebuffer->height(),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    VkRect2D scissor{
        .offset = {0, 0},
        .extent = {activeFramebuffer->width(), activeFramebuffer->height()}
    };

    vkCmdSetViewport(_getActiveCommandBuffer(), 0, 1, &viewport);
    vkCmdSetScissor(_getActiveCommandBuffer(), 0, 1, &scissor);
}

void CommandBuffer::cmdPushConstants(void* data, uint16_t index) {
    vkCmdPushConstants(_getActiveCommandBuffer(),
                       activePipelineLayout->pipelineLayout(),
                       activePipelineLayout->pushConstantRange(index).stageFlags,
                       activePipelineLayout->pushConstantRange(index).offset,
                       activePipelineLayout->pushConstantRange(index).size, data);
}

void CommandBuffer::cmdBindViewport(Viewport* viewport) {
    vkCmdSetViewport(_getActiveCommandBuffer(), 0, 1, &viewport->viewport);
	vkCmdSetScissor(_getActiveCommandBuffer(), 0, 1, &viewport->scissor);
}

//Compute
void CommandBuffer::cmdBindComputePipeline(ComputePipeline* computePipeline) {
    vkCmdBindPipeline(_getActiveCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline->computePipeline());
    pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

void CommandBuffer::cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ, uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ) {
    vkCmdDispatch(_getActiveCommandBuffer(), threadgroupsX, threadgroupsY, threadgroupsZ);
}

//Blit
void CommandBuffer::cmdStagingCopyDataToBuffer(Buffer* buffer, void* data, size_t aSize) {
    if (aSize == 0)
        aSize = buffer->size();
    uint8_t index = g_vulkan_swapChain->crntFrame() % buffer->frameCount();
    
    VkBuffer stagingBuffer;

    VmaAllocation stagingAllocation = BufferHelper::createBuffer(aSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

    void* mappedData;
    vmaMapMemory(g_vulkan_device->allocator(), stagingAllocation, &mappedData);
    memcpy(mappedData, data, aSize);
    vmaUnmapMemory(g_vulkan_device->allocator(), stagingAllocation);

    BufferHelper::copyBuffer(_getActiveCommandBuffer(), stagingBuffer, buffer->buffer(index), aSize);

    //vmaDestroyBuffer(g_vulkan_device->allocator(), stagingBuffer, stagingAllocation);
    stagingBufferAllocationsToDestroy.emplace_back(BufferAllocation{stagingBuffer, stagingAllocation});
}

void CommandBuffer::cmdStagingCopyDataToImage(Image* image, void* data, uint8_t bytesPerPixel) {
    VkDeviceSize imageSize = image->width() * image->height() * bytesPerPixel;

    VkBuffer stagingBuffer;

    VmaAllocation stagingAllocation = BufferHelper::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedData;
    vmaMapMemory(g_vulkan_device->allocator(), stagingAllocation, &mappedData);
    memcpy(mappedData, data, imageSize);
    vmaUnmapMemory(g_vulkan_device->allocator(), stagingAllocation);

    for (uint8_t i = 0; i < image->frameCount(); i++) {
        cmdTransitionImageLayout(image, i, ImageLayout::Undefined, ImageLayout::TransferDestinationOptimal);
        BufferHelper::copyBufferToImage(_getActiveCommandBuffer(), stagingBuffer, image->image(i), image->width(), image->height());
    }

    //vmaDestroyBuffer(g_vulkan_device->allocator(), stagingBuffer, stagingAllocation);
    stagingBufferAllocationsToDestroy.emplace_back(BufferAllocation{stagingBuffer, stagingAllocation});
}

void CommandBuffer::cmdTransitionImageLayout(Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout) {
    VkFormat vkFormat;
    GET_VK_FORMAT(image->format(), vkFormat);
    VkImageLayout vkSourceImageLayout, vkDestinationImageLayout;
    GET_VK_IMAGE_LAYOUT(srcLayout, vkSourceImageLayout);
    GET_VK_IMAGE_LAYOUT(dstLayout, vkDestinationImageLayout);

    ImageHelper::transitionImageLayout(_getActiveCommandBuffer(), image->image(imageIndex), vkFormat, vkSourceImageLayout, vkDestinationImageLayout, getVKImageAspectFlags(image->aspect()), image->layerCount(), image->mipCount());
}

void CommandBuffer::cmdGenerateMipmapsForImage(Image* image, uint8_t aFrameCount) {
    if (aFrameCount == 0) aFrameCount = image->frameCount();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    for (uint8_t i = 0; i < aFrameCount; i++) {
        uint8_t index = g_vulkan_swapChain->crntFrame() + i;
        barrier.image = image->image(index);

        int32_t mipWidth = image->width();
        int32_t mipHeight = image->height();

        for (uint32_t mip = 0; mip < image->mipCount() - 1; mip++) {
            barrier.subresourceRange.baseMipLevel = mip;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(_getActiveCommandBuffer(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = mip;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;

            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = mip + 1;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(_getActiveCommandBuffer(),
                image->image(index), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image->image(index), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);
            
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(_getActiveCommandBuffer(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }
    }
}

void CommandBuffer::cmdCopyToImageFromImage(Image* source, Image* destination) {
    throw std::runtime_error("Not implemented yet");
}

void CommandBuffer::cmdBlitToImageFromImage(Image* source, Image* destination) {
    uint8_t srcIndex = g_vulkan_swapChain->crntFrame() % source->frameCount();
    uint8_t dstIndex = g_vulkan_swapChain->crntFrame() % destination->frameCount();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = getVKImageAspectFlags(destination->aspect());
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;

    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.image = source->image(srcIndex);

    vkCmdPipelineBarrier(_getActiveCommandBuffer(),
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.image = destination->image(dstIndex);

    vkCmdPipelineBarrier(_getActiveCommandBuffer(),
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {source->width(), source->height(), 1};
    blit.srcSubresource.aspectMask = getVKImageAspectFlags(source->aspect());
    blit.srcSubresource.mipLevel = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;

    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {destination->width(), destination->height(), 1 };
    blit.dstSubresource.aspectMask = getVKImageAspectFlags(destination->aspect());
    blit.dstSubresource.mipLevel = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(_getActiveCommandBuffer(),
        source->image(srcIndex), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        destination->image(dstIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &blit,
        VK_FILTER_NEAREST);
    
    /*
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.image = images[dstIndex];

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
    */
}

} //namespace vulkan

} //namespace lv
