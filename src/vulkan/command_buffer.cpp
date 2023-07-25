#include "lvcore/vulkan/command_buffer.hpp"

#include "lvcore/vulkan/core.hpp"

#include "lvcore/vulkan/swap_chain.hpp"

namespace lv {

namespace vulkan {

#define _LV_CHECK_IF_ENCODING \
if (renderPassBound) { \
    vkCmdEndRenderPass(_getActiveCommandBuffer()); \
    renderPassBound = false; \
}

CommandBuffer::CommandBuffer(internal::CommandBufferCreateInfo createInfo) {
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

void CommandBuffer::beginRecording(CommandBufferUsageFlags usage) {
    uint8_t index = g_vulkan_swapChain->crntFrame() % frameCount;
    index = g_vulkan_swapChain->crntFrame();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = getVKCommandBufferUsageFlags(usage);

    VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[index], &beginInfo));
}

void CommandBuffer::endRecording() {
    _LV_CHECK_IF_ENCODING;

    uint8_t index = g_vulkan_swapChain->crntFrame() % frameCount;
    index = g_vulkan_swapChain->crntFrame();
    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[index]));
}

void CommandBuffer::submit(internal::Semaphore* waitSemaphore, internal::Semaphore* signalSemaphore) {
    CAST_FROM_INTERNAL(waitSemaphore, Semaphore);
    CAST_FROM_INTERNAL(signalSemaphore, Semaphore);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[g_vulkan_swapChain->crntFrame() % frameCount];
    if (waitSemaphore_ != nullptr) {
        VkSemaphore semaphores[] = {waitSemaphore_->semaphore(g_vulkan_swapChain->crntFrame())}; //TODO: make this image count for swap chain semaphores
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = semaphores;
    }
    if (signalSemaphore_ != nullptr) {
        VkSemaphore semaphores[] = {signalSemaphore_->semaphore(g_vulkan_swapChain->crntFrame())};
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

void CommandBuffer::beginRenderCommands(internal::Framebuffer* framebuffer) {
    CAST_FROM_INTERNAL(framebuffer, Framebuffer);

    _LV_CHECK_IF_ENCODING;
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = framebuffer_->renderPass()->renderPass();
    renderPassInfo.framebuffer = framebuffer_->framebuffer(g_vulkan_swapChain->crntFrame()); //TODO: use image index in case of swap chain framebuffer
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VkExtent2D{framebuffer_->width(), framebuffer_->height()};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(framebuffer_->clearValueCount());
    renderPassInfo.pClearValues = framebuffer_->clearValuesData();

    vkCmdBeginRenderPass(_getActiveCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    activeFramebuffer = framebuffer_;

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
void CommandBuffer::cmdBindVertexBuffer(internal::Buffer* buffer) {
    CAST_FROM_INTERNAL(buffer, Buffer);

    VkDeviceSize offsets[] = {0};
    VkBuffer buffers[] = {buffer_->buffer(g_vulkan_swapChain->crntFrame() % buffer_->frameCount())};
    vkCmdBindVertexBuffers(_getActiveCommandBuffer(), 0, 1, buffers, offsets);
}

void CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount) {
    vkCmdDraw(_getActiveCommandBuffer(), vertexCount, instanceCount, 0, 0);
}

void CommandBuffer::cmdDrawIndexed(internal::Buffer* indexBuffer, IndexType indexType, uint32_t indexCount, uint32_t instanceCount) {
    CAST_FROM_INTERNAL(indexBuffer, Buffer);

    VkIndexType vkIndexType;
    GET_VK_INDEX_TYPE(indexType, vkIndexType);
    
    vkCmdBindIndexBuffer(_getActiveCommandBuffer(), indexBuffer_->buffer(g_vulkan_swapChain->crntFrame() % indexBuffer_->frameCount()), 0, vkIndexType);

    vkCmdDrawIndexed(_getActiveCommandBuffer(), indexCount, instanceCount, 0, 0, 0);
}

void CommandBuffer::cmdBindDescriptorSet(internal::DescriptorSet* descriptorSet) {
    CAST_FROM_INTERNAL(descriptorSet, DescriptorSet);

    VkDescriptorSet descriptorSets[] = {descriptorSet_->descriptorSet(g_vulkan_swapChain->crntFrame() % descriptorSet_->frameCount())};
    vkCmdBindDescriptorSets(_getActiveCommandBuffer(), pipelineBindPoint, descriptorSet_->pipelineLayout()->pipelineLayout(), descriptorSet_->layoutIndex(), 1, descriptorSets, 0, nullptr);
}

void CommandBuffer::cmdNextSubpass() {
    vkCmdNextSubpass(_getActiveCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::cmdBindGraphicsPipeline(internal::GraphicsPipeline* graphicsPipeline) {
    CAST_FROM_INTERNAL(graphicsPipeline, GraphicsPipeline);

    vkCmdBindPipeline(_getActiveCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_->graphicsPipeline());
    pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    activePipelineLayout = graphicsPipeline_->pipelineLayout();
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

void CommandBuffer::cmdBindViewport(internal::Viewport* viewport) {
    CAST_FROM_INTERNAL(viewport, Viewport);

    vkCmdSetViewport(_getActiveCommandBuffer(), 0, 1, &viewport_->getViewport());
	vkCmdSetScissor(_getActiveCommandBuffer(), 0, 1, &viewport_->getScissor());
}

//Compute
void CommandBuffer::cmdBindComputePipeline(internal::ComputePipeline* computePipeline) {
    CAST_FROM_INTERNAL(computePipeline, ComputePipeline);

    vkCmdBindPipeline(_getActiveCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline_->computePipeline());
    pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

void CommandBuffer::cmdDispatchThreadgroups(uint32_t threadgroupsX, uint32_t threadgroupsY, uint32_t threadgroupsZ, uint32_t threadsPerGroupX, uint32_t threadsPerGroupY, uint32_t threadsPerGroupZ) {
    vkCmdDispatch(_getActiveCommandBuffer(), threadgroupsX, threadgroupsY, threadgroupsZ);
}

//Blit
void CommandBuffer::cmdStagingCopyDataToBuffer(internal::Buffer* buffer, void* data, size_t aSize) {
    CAST_FROM_INTERNAL(buffer, Buffer);

    if (aSize == 0)
        aSize = buffer_->size();
    uint8_t index = g_vulkan_swapChain->crntFrame() % buffer_->frameCount();
    
    VkBuffer stagingBuffer;

    VmaAllocation stagingAllocation = BufferHelper::createBuffer(aSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

    void* mappedData;
    vmaMapMemory(g_vulkan_device->allocator(), stagingAllocation, &mappedData);
    memcpy(mappedData, data, aSize);
    vmaUnmapMemory(g_vulkan_device->allocator(), stagingAllocation);

    BufferHelper::copyBuffer(_getActiveCommandBuffer(), stagingBuffer, buffer_->buffer(index), aSize);

    //vmaDestroyBuffer(g_vulkan_device->allocator(), stagingBuffer, stagingAllocation);
    stagingBufferAllocationsToDestroy.emplace_back(BufferAllocation{stagingBuffer, stagingAllocation});
}

void CommandBuffer::cmdStagingCopyDataToImage(internal::Image* image, void* data, uint8_t bytesPerPixel) {
    CAST_FROM_INTERNAL(image, Image);

    VkDeviceSize imageSize = image_->width() * image_->height() * bytesPerPixel;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation = BufferHelper::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedData;
    vmaMapMemory(g_vulkan_device->allocator(), stagingAllocation, &mappedData);
    memcpy(mappedData, data, imageSize);
    vmaUnmapMemory(g_vulkan_device->allocator(), stagingAllocation);

    for (uint8_t i = 0; i < image_->frameCount(); i++) {
        cmdTransitionImageLayout(image_, i, ImageLayout::Undefined, ImageLayout::TransferDestinationOptimal);
        BufferHelper::copyBufferToImage(_getActiveCommandBuffer(), stagingBuffer, image_->image(i), image_->width(), image_->height());
    }

    //vmaDestroyBuffer(g_vulkan_device->allocator(), stagingBuffer, stagingAllocation);
    stagingBufferAllocationsToDestroy.emplace_back(BufferAllocation{stagingBuffer, stagingAllocation});
}

void CommandBuffer::cmdTransitionImageLayout(internal::Image* image, uint8_t imageIndex, ImageLayout srcLayout, ImageLayout dstLayout) {
    CAST_FROM_INTERNAL(image, Image);

    VkFormat vkFormat;
    GET_VK_FORMAT(image_->format(), vkFormat);
    VkImageLayout vkSourceImageLayout, vkDestinationImageLayout;
    GET_VK_IMAGE_LAYOUT(srcLayout, vkSourceImageLayout);
    GET_VK_IMAGE_LAYOUT(dstLayout, vkDestinationImageLayout);

    ImageHelper::transitionImageLayout(_getActiveCommandBuffer(), image_->image(imageIndex), vkFormat, vkSourceImageLayout, vkDestinationImageLayout, getVKImageAspectFlags(image_->aspect()), image_->layerCount(), image_->mipCount());
}

void CommandBuffer::cmdGenerateMipmapsForImage(internal::Image* image, uint8_t aFrameCount) {
    CAST_FROM_INTERNAL(image, Image);

    if (aFrameCount == 0) aFrameCount = image_->frameCount();

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
        barrier.image = image_->image(index);

        int32_t mipWidth = image_->width();
        int32_t mipHeight = image_->height();

        for (uint32_t mip = 0; mip < image_->mipCount() - 1; mip++) {
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
                image_->image(index), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image_->image(index), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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

void CommandBuffer::cmdCopyToImageFromImage(internal::Image* source, internal::Image* destination) {
    throw std::runtime_error("Not implemented yet");
}

void CommandBuffer::cmdBlitToImageFromImage(internal::Image* source, internal::Image* destination) {
    CAST_FROM_INTERNAL(source, Image);
    CAST_FROM_INTERNAL(destination, Image);


    uint8_t srcIndex = g_vulkan_swapChain->crntFrame() % source_->frameCount();
    uint8_t dstIndex = g_vulkan_swapChain->crntFrame() % destination_->frameCount();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = getVKImageAspectFlags(destination_->aspect());
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;

    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.image = source_->image(srcIndex);

    vkCmdPipelineBarrier(_getActiveCommandBuffer(),
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.image = destination_->image(dstIndex);

    vkCmdPipelineBarrier(_getActiveCommandBuffer(),
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {source_->width(), source_->height(), 1};
    blit.srcSubresource.aspectMask = getVKImageAspectFlags(source_->aspect());
    blit.srcSubresource.mipLevel = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;

    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {destination_->width(), destination_->height(), 1 };
    blit.dstSubresource.aspectMask = getVKImageAspectFlags(destination_->aspect());
    blit.dstSubresource.mipLevel = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(_getActiveCommandBuffer(),
        source_->image(srcIndex), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        destination_->image(dstIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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
