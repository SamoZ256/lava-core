#include "vulkan/lvcore/core/core.hpp"

namespace lv {

namespace vulkan {

//---------------- Shader stage ----------------
VkShaderStageFlags getVKShaderStageFlags(ShaderStageFlags shaderStageFlags) {
    VkShaderStageFlags vkShaderStage = 0;

    if (shaderStageFlags & ShaderStageFlags::Vertex)
        vkShaderStage |= VK_SHADER_STAGE_VERTEX_BIT;
    if (shaderStageFlags & ShaderStageFlags::Fragment)
        vkShaderStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if (shaderStageFlags & ShaderStageFlags::Compute)
        vkShaderStage |= VK_SHADER_STAGE_COMPUTE_BIT;
    
    return vkShaderStage;
}

VkShaderStageFlagBits getVKShaderStageFlagBits(ShaderStageFlags shaderStageFlags) {
    if (shaderStageFlags & ShaderStageFlags::Vertex)
        return VK_SHADER_STAGE_VERTEX_BIT;
    if (shaderStageFlags & ShaderStageFlags::Fragment)
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    if (shaderStageFlags & ShaderStageFlags::Compute)
        return VK_SHADER_STAGE_COMPUTE_BIT;
    
    LV_INVALID_ARGUMENT("shaderStageFlags");
    
    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

//---------------- Image usage ----------------
VkImageUsageFlags getVKImageUsageFlags(ImageUsageFlags imageUsageFlags) {
    VkImageUsageFlags vkImageUsage = 0;

    if (imageUsageFlags & ImageUsageFlags::Sampled)
        vkImageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (imageUsageFlags & ImageUsageFlags::ColorAttachment)
        vkImageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (imageUsageFlags & ImageUsageFlags::DepthStencilAttachment)
        vkImageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (imageUsageFlags & ImageUsageFlags::TransientAttachment)
        vkImageUsage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    if (imageUsageFlags & ImageUsageFlags::InputAttachment)
        vkImageUsage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    if (imageUsageFlags & ImageUsageFlags::StorageImage)
        vkImageUsage |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (imageUsageFlags & ImageUsageFlags::TransferSource)
        vkImageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (imageUsageFlags & ImageUsageFlags::TransferDestination)
        vkImageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    
    return vkImageUsage;
}

//---------------- Image aspect ----------------
VkImageAspectFlags getVKImageAspectFlags(ImageAspectFlags imageAspectFlags) {
    VkImageAspectFlags vkImageAspect = 0;

    if (imageAspectFlags & ImageAspectFlags::Color)
        vkImageAspect |= VK_IMAGE_ASPECT_COLOR_BIT;
    if (imageAspectFlags & ImageAspectFlags::Depth)
        vkImageAspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (imageAspectFlags & ImageAspectFlags::Stencil)
        vkImageAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
    
    return vkImageAspect;
}

//---------------- Command buffer usage ----------------
VkCommandBufferUsageFlags getVKCommandBufferUsageFlags(CommandBufferUsageFlags commandBufferUsageFlags) {
    VkCommandBufferUsageFlags vkCommandBufferUsage;

    if (commandBufferUsageFlags & CommandBufferUsageFlags::OneTimeSubmit)
        vkCommandBufferUsage |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    return vkCommandBufferUsage;
}

//---------------- Buffer usage ----------------
VkBufferUsageFlags getVKBufferUsageFlags(BufferUsageFlags bufferUsageFlags) {
    VkBufferUsageFlags vkBufferUsage = 0;

    if (bufferUsageFlags & BufferUsageFlags::TransferSource)
        vkBufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if (bufferUsageFlags & BufferUsageFlags::TransferDestination)
        vkBufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (bufferUsageFlags & BufferUsageFlags::UniformBuffer)
        vkBufferUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (bufferUsageFlags & BufferUsageFlags::StorageBuffer)
        vkBufferUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (bufferUsageFlags & BufferUsageFlags::IndexBuffer)
        vkBufferUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (bufferUsageFlags & BufferUsageFlags::VertexBuffer)
        vkBufferUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    return vkBufferUsage;
}

//---------------- Allocation create ----------------
VmaAllocationCreateFlags getVKAllocationCreateFlags(MemoryAllocationCreateFlags memoryAllocationCreateFlags) {
    VmaAllocationCreateFlags vkAllocationCreate = 0;

    if (memoryAllocationCreateFlags & MemoryAllocationCreateFlags::Dedicated)
        vkAllocationCreate |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    if (memoryAllocationCreateFlags & MemoryAllocationCreateFlags::MinMemory)
        vkAllocationCreate |= VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT;

    return vkAllocationCreate;
}

} //namespace vulkan

} //namespace lv
