#ifndef LV_VULKAN_CORE_H
#define LV_VULKAN_CORE_H

#include <vk_mem_alloc.h>

#include "lvcore/internal/common.hpp"

//TODO: remove the use of 'vulkan' namespace from macros

namespace lv {

namespace vulkan {

//---------------- Format ----------------
constexpr VkFormat formatLUT[] = {
    //---------------- 8 ----------------

    //R
    VK_FORMAT_R8_UINT,
    VK_FORMAT_R8_SINT,
    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8_SNORM,
    VK_FORMAT_R8_SRGB,

    //S
    VK_FORMAT_S8_UINT,

    //---------------- 16 ----------------

    //R
    VK_FORMAT_R16_UINT,
    VK_FORMAT_R16_SINT,
    VK_FORMAT_R16_UNORM,
    VK_FORMAT_R16_SNORM,
    VK_FORMAT_R16_SFLOAT,

    //RG
    VK_FORMAT_R8G8_UINT,
    VK_FORMAT_R8G8_SINT,
    VK_FORMAT_R8G8_UNORM,
    VK_FORMAT_R8G8_SNORM,
    VK_FORMAT_R8G8_SRGB,

    //D
    VK_FORMAT_D16_UNORM,

    //BGR - packed
    VK_FORMAT_B5G6R5_UNORM_PACK16,

    //ABGR - packed
    VK_FORMAT_A1R5G5B5_UNORM_PACK16,

    //BGRA - packed
    VK_FORMAT_A4B4G4R4_UNORM_PACK16,
    VK_FORMAT_B5G5R5A1_UNORM_PACK16,

    //---------------- 24 ----------------

    //RGB
    VK_FORMAT_R8G8B8_UINT,
    VK_FORMAT_R8G8B8_SINT,
    VK_FORMAT_R8G8B8_UNORM,
    VK_FORMAT_R8G8B8_SNORM,

    //---------------- 32 ----------------

    //R
    VK_FORMAT_R32_UINT,
    VK_FORMAT_R32_SINT,
    VK_FORMAT_R32_SFLOAT,

    //RG
    VK_FORMAT_R16G16_UINT,
    VK_FORMAT_R16G16_SINT,
    VK_FORMAT_R16G16_UNORM,
    VK_FORMAT_R16G16_SNORM,
    VK_FORMAT_R16G16_SFLOAT,

    //RGBA
    VK_FORMAT_R8G8B8A8_UINT,
    VK_FORMAT_R8G8B8A8_SINT,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8G8B8A8_SRGB,

    //BGRA
    VK_FORMAT_B8G8R8A8_SRGB,

    //D
    VK_FORMAT_D32_SFLOAT,

    //D_S
    VK_FORMAT_D24_UNORM_S8_UINT,

    //BGR - packed
    VK_FORMAT_B10G11R11_UFLOAT_PACK32,
    
    //ABGR - packed
    VK_FORMAT_A2B10G10R10_UNORM_PACK32,

    //ARGB - packed
    VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    VK_FORMAT_A2R10G10B10_SNORM_PACK32,
    VK_FORMAT_A2R10G10B10_UINT_PACK32,

    //ERGB - packed
    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,

    //---------------- 40 ----------------

    //D_S
    VK_FORMAT_D32_SFLOAT_S8_UINT,

    //---------------- 48 ----------------

    //RGB
    VK_FORMAT_R16G16B16_UINT, //Invalid
    VK_FORMAT_R16G16B16_SINT, //Invalid
    VK_FORMAT_R16G16B16_UNORM, //Invalid
    VK_FORMAT_R16G16B16_SNORM, //Invalid
    VK_FORMAT_R16G16B16_SFLOAT, //Invalid

    //---------------- 64 ----------------

    //RG
    VK_FORMAT_R32G32_UINT,
    VK_FORMAT_R32G32_SINT,
    VK_FORMAT_R32G32_SFLOAT,

    //RGBA
    VK_FORMAT_R16G16B16A16_UINT,
    VK_FORMAT_R16G16B16A16_SINT,
    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R16G16B16A16_SNORM,
    VK_FORMAT_R16G16B16A16_SFLOAT,

    //---------------- 96 ----------------

    //RGB
    VK_FORMAT_R32G32B32_UINT,
    VK_FORMAT_R32G32B32_SINT,
    VK_FORMAT_R32G32B32_SFLOAT,

    //---------------- 128 ----------------

    //RGBA
    VK_FORMAT_R32G32B32A32_UINT,
    VK_FORMAT_R32G32B32A32_SINT,
    VK_FORMAT_R32G32B32A32_SFLOAT,

    //---------------- ASTC ----------------

    VK_FORMAT_UNDEFINED, //Invalid
    VK_FORMAT_UNDEFINED  //Invalid
};

#define GET_VK_FORMAT(format, retval) \
LV_CHECK_ARGUMENT(Format, format); \
retval = vulkan::formatLUT[(int)format];

//---------------- Cull mode ----------------
constexpr VkCullModeFlags cullModeLUT[] = {
    VK_CULL_MODE_NONE,
    VK_CULL_MODE_FRONT_BIT,
    VK_CULL_MODE_BACK_BIT
};

#define GET_VK_CULL_MODE(cullMode, retval) \
LV_CHECK_ARGUMENT(CullMode, cullMode); \
retval = vulkan::cullModeLUT[(int)cullMode];

//---------------- Front face ----------------
constexpr VkFrontFace frontFaceLUT[] = {
    VK_FRONT_FACE_CLOCKWISE,
    VK_FRONT_FACE_COUNTER_CLOCKWISE
};

#define GET_VK_FRONT_FACE(frontFace, retval) \
LV_CHECK_ARGUMENT(FrontFace, frontFace); \
retval = vulkan::frontFaceLUT[(int)frontFace];

//---------------- Filter ----------------
constexpr VkFilter filterLUT[] = {
    VK_FILTER_NEAREST,
    VK_FILTER_LINEAR
};

#define GET_VK_FILTER(filter, retval) \
LV_CHECK_ARGUMENT(Filter, filter); \
retval = vulkan::filterLUT[(int)filter];

//---------------- Image type ----------------
constexpr VkImageType imageTypeLUT[] = {
    VK_IMAGE_TYPE_1D,
    VK_IMAGE_TYPE_2D,
    VK_IMAGE_TYPE_3D,
    VK_IMAGE_TYPE_2D,
    VK_IMAGE_TYPE_1D,
    VK_IMAGE_TYPE_2D,
    VK_IMAGE_TYPE_2D //TODO: check if cube array is 2D image
};

#define GET_VK_IMAGE_TYPE(imageType, retval) \
LV_CHECK_ARGUMENT(ImageType, imageType); \
retval = vulkan::imageTypeLUT[(int)imageType];

//---------------- Image view type ----------------
constexpr VkImageViewType imageViewTypeLUT[] = {
    VK_IMAGE_VIEW_TYPE_1D,
    VK_IMAGE_VIEW_TYPE_2D,
    VK_IMAGE_VIEW_TYPE_3D,
    VK_IMAGE_VIEW_TYPE_CUBE,
    VK_IMAGE_VIEW_TYPE_1D_ARRAY,
    VK_IMAGE_VIEW_TYPE_2D_ARRAY,
    VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
};

#define GET_VK_IMAGE_VIEW_TYPE(imageType, retval) \
LV_CHECK_ARGUMENT(ImageType, imageType); \
retval = vulkan::imageViewTypeLUT[(int)imageType];

//---------------- Sampler address mode ----------------
constexpr VkSamplerAddressMode samplerAddressModeLUT[] = {
    VK_SAMPLER_ADDRESS_MODE_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

#define GET_VK_SAMPLER_ADDRESS_MODE(samplerAddressMode, retval) \
LV_CHECK_ARGUMENT(SamplerAddressMode, samplerAddressMode); \
retval = vulkan::samplerAddressModeLUT[(int)samplerAddressMode];

//---------------- Index type ----------------
constexpr VkIndexType indexTypeLUT[] = {
    VK_INDEX_TYPE_UINT16,
    VK_INDEX_TYPE_UINT32
};

#define GET_VK_INDEX_TYPE(indexType, retval) \
LV_CHECK_ARGUMENT(IndexType, indexType); \
retval = vulkan::indexTypeLUT[(int)indexType];

//---------------- Attachment load op ----------------
constexpr VkAttachmentLoadOp attachmentLoadOpLUT[] = {
    VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    VK_ATTACHMENT_LOAD_OP_CLEAR,
    VK_ATTACHMENT_LOAD_OP_LOAD
};

#define GET_VK_ATTACHMENT_LOAD_OP(attachmentLoadOperation, retval) \
LV_CHECK_ARGUMENT(AttachmentLoadOperation, attachmentLoadOperation); \
retval = vulkan::attachmentLoadOpLUT[(int)attachmentLoadOperation];

//---------------- Attachment store op ----------------
constexpr VkAttachmentStoreOp attachmentStoreOpLUT[] = {
    VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_ATTACHMENT_STORE_OP_STORE
};

#define GET_VK_ATTACHMENT_STORE_OP(attachmentStoreOperation, retval) \
LV_CHECK_ARGUMENT(AttachmentStoreOperation, attachmentStoreOperation); \
retval = vulkan::attachmentStoreOpLUT[(int)attachmentStoreOperation];

//---------------- Compare op ----------------
constexpr VkCompareOp compareOpLUT[] = {
    VK_COMPARE_OP_NEVER,
    VK_COMPARE_OP_LESS,
    VK_COMPARE_OP_EQUAL,
    VK_COMPARE_OP_LESS_OR_EQUAL,
    VK_COMPARE_OP_GREATER,
    VK_COMPARE_OP_NOT_EQUAL,
    VK_COMPARE_OP_GREATER_OR_EQUAL,
    VK_COMPARE_OP_ALWAYS
};

#define GET_VK_COMPARE_OP(compareOperation, retval) \
LV_CHECK_ARGUMENT(CompareOperation, compareOperation); \
retval = vulkan::compareOpLUT[(int)compareOperation];

//---------------- Image layout ----------------
constexpr VkImageLayout imageLayoutLUT[] = {
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_GENERAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};

#define GET_VK_IMAGE_LAYOUT(imageLayout, retval) \
LV_CHECK_ARGUMENT(ImageLayout, imageLayout); \
retval = vulkan::imageLayoutLUT[(int)imageLayout];

//---------------- Memory properties ----------------
constexpr VkMemoryPropertyFlags memoryPropertiesLUT[] = {
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
};

#define GET_VK_MEMORY_PROPERTIES(memoryType, retval) \
LV_CHECK_ARGUMENT(MemoryType, memoryType); \
retval = vulkan::memoryPropertiesLUT[(int)memoryType];

//---------------- Blend op ----------------
constexpr VkBlendOp blendOpLUT[] = {
    VK_BLEND_OP_ADD,
    VK_BLEND_OP_SUBTRACT,
    VK_BLEND_OP_REVERSE_SUBTRACT,
    VK_BLEND_OP_MIN,
    VK_BLEND_OP_MAX
};

#define GET_VK_BLEND_OPERATION(blendOperation, retval) \
LV_CHECK_ARGUMENT(BlendOperation, blendOperation); \
retval = vulkan::blendOpLUT[(int)blendOperation];

//---------------- Blend factor ----------------
constexpr VkBlendFactor blendFactorLUT[] = {
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_SRC_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    VK_BLEND_FACTOR_DST_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_DST_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    VK_BLEND_FACTOR_CONSTANT_COLOR,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    VK_BLEND_FACTOR_CONSTANT_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA
};

#define GET_VK_BLEND_FACTOR(blendFactor, retval) \
LV_CHECK_ARGUMENT(BlendFactor, blendFactor); \
retval = vulkan::blendFactorLUT[(int)blendFactor];

//---------------- Descriptor type ----------------
constexpr VkDescriptorType descriptorTypeLUT[] = {
    VK_DESCRIPTOR_TYPE_SAMPLER,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
};

#define GET_VK_DESCRIPTOR_TYPE(descriptorType, retval) \
LV_CHECK_ARGUMENT(DescriptorType, descriptorType); \
retval = vulkan::descriptorTypeLUT[(int)descriptorType];

//---------------- Vertex input rate ----------------
constexpr VkVertexInputRate vertexInputRateLUT[] = {
    VK_VERTEX_INPUT_RATE_MAX_ENUM,
    VK_VERTEX_INPUT_RATE_VERTEX,
    VK_VERTEX_INPUT_RATE_INSTANCE,
    VK_VERTEX_INPUT_RATE_MAX_ENUM,
    VK_VERTEX_INPUT_RATE_MAX_ENUM
};

#define GET_VK_VERTEX_INPUT_RATE(vertexInputRate, retval) \
LV_CHECK_ARGUMENT(VertexInputRate, vertexInputRate); \
retval = vulkan::vertexInputRateLUT[(int)vertexInputRate];

//---------------- Flags ----------------
VkShaderStageFlags getVKShaderStageFlags(ShaderStageFlags shaderStageFlags);
VkShaderStageFlagBits getVKShaderStageFlagBits(ShaderStageFlags shaderStageFlags);
VkImageUsageFlags getVKImageUsageFlags(ImageUsageFlags imageUsageFlags);
VkImageAspectFlags getVKImageAspectFlags(ImageAspectFlags imageAspectFlags);
VkCommandBufferUsageFlags getVKCommandBufferUsageFlags(CommandBufferUsageFlags commandBufferUsageFlags);
VkBufferUsageFlags getVKBufferUsageFlags(BufferUsageFlags bufferUsageFlags);
VmaAllocationCreateFlags getVKAllocationCreateFlags(MemoryAllocationCreateFlags memoryAllocationCreateFlags);

} //namespace vulkan

} //namespace lv

#endif
