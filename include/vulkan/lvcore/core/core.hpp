#ifndef LV_VULKAN_CORE_H
#define LV_VULKAN_CORE_H

#include <vulkan/vulkan.h>

#include "lvcore/core/common.hpp"

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

//---------------- Attachment load op ----------------
constexpr VkAttachmentStoreOp attachmentStoreOpLUT[] = {
    VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_ATTACHMENT_STORE_OP_STORE
};

#define GET_VK_ATTACHMENT_STORE_OP(attachmentStoreOperation, retval) \
LV_CHECK_ARGUMENT(AttachmentStoreOperation, attachmentStoreOperation); \
retval = vulkan::attachmentStoreOpLUT[(int)attachmentStoreOperation];

} //namespace vulkan

} //namespace lv

#endif
