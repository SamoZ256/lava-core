#ifndef LV_METAL_ENUMS_H
#define LV_METAL_ENUMS_H

#include <stdint.h>

//#include "common.hpp"

//#include <Metal/Metal.hpp>

//Types
/*
#ifdef __OBJC__

#define MTL_RENDER_COMMAND_ENCODER_T id<MTLRenderCommandEncoder>

#else

#define MTL_RENDER_COMMAND_ENCODER_T id

#endif
*/

//Bool
#define LvBool bool

#define LV_TRUE true
#define LV_FALSE false

//Shader stage
#define LvShaderStageFlags uint8_t

#define LV_SHADER_STAGE_VERTEX_BIT 0x1
#define LV_SHADER_STAGE_TESSELLATION_CONTROL_BIT LV_SHADER_STAGE_COMPUTE_BIT
#define LV_SHADER_STAGE_TESSELLATION_EVALUATION_BIT LV_SHADER_STAGE_VERTEX_BIT
#define LV_SHADER_STAGE_FRAGMENT_BIT 0x2
#define LV_SHADER_STAGE_COMPUTE_BIT 0x4

#define LV_SHADER_STAGE_VERTEX_INDEX 0
#define LV_SHADER_STAGE_FRAGMENT_INDEX 1
#define LV_SHADER_STAGE_COMPUTE_INDEX 2

//Format
#define LvFormat uint16_t /*MTL::PixelFormat*/

//R

//8
#define LV_FORMAT_R8_UINT 13 /*MTLPixelFormatR8Uint*/
#define LV_FORMAT_R8_SINT 14 /*MTLPixelFormatR8Sint*/
#define LV_FORMAT_R8_UNORM 10 /*MTLPixelFormatR8Unorm*/
#define LV_FORMAT_R8_SNORM 12 /*MTLPixelFormatR8Snorm*/
#define LV_FORMAT_R8_UNORM_SRGB 11 /*MTLPixelFormatR8Unorm_sRGB*/
    
//16
#define LV_FORMAT_R16_UINT 23 /*MTLPixelFormatR16Uint*/
#define LV_FORMAT_R16_SINT 24 /*MTLPixelFormatR16Sint*/
#define LV_FORMAT_R16_UNORM 20 /*MTLPixelFormatR16Unorm*/
#define LV_FORMAT_R16_SNORM 22 /*MTLPixelFormatR16Snorm*/

//32
#define LV_FORMAT_R32_UINT 53 /*MTLPixelFormatR32Uint*/
#define LV_FORMAT_R32_SINT 54 /*MTLPixelFormatR32Sint*/
#define LV_FORMAT_R32_SFLOAT 55 /*MTLPixelFormatR32Float*/

//RG

//8
#define LV_FORMAT_R8G8_UINT 33 /*MTL::PixelFormatRG8Uint*/
#define LV_FORMAT_R8G8_SINT 34 /*MTL::PixelFormatRG8Sint*/
#define LV_FORMAT_R8G8_UNORM 30 /*MTL::PixelFormatRG8Unorm*/
#define LV_FORMAT_R8G8_SNORM 32 /*MTL::PixelFormatRG8Snorm*/
#define LV_FORMAT_R8G8_UNORM_SRGB 31 /*MTL::PixelFormatRG8Unorm_sRGB*/
    
//16
#define LV_FORMAT_R16G16_UINT 63 /*MTL::PixelFormatRG16Uint*/
#define LV_FORMAT_R16G16_SINT 64 /*MTL::PixelFormatRG16Sint*/
#define LV_FORMAT_R16G16_UNORM 60 /*MTL::PixelFormatRG16Unorm*/
#define LV_FORMAT_R16G16_SNORM 62 /*MTL::PixelFormatRG16Snorm*/

//32
#define LV_FORMAT_R32G32_UINT 103 /*MTL::PixelFormatRG32Uint*/
#define LV_FORMAT_R32G32_SINT 104 /*MTL::PixelFormatRG32Sint*/

//RGB

//Packed
#define LV_FORMAT_B5G6R5_UNORM 40 /*MTL::PixelFormatB5G6R5Unorm*/

#define LV_FORMAT_B10G11R11_UFLOAT 92 /*MTL::PixelFormatRG11B10Float*/
#define LV_FORMAT_E5R9G9B9_UFLOAT 93 /*MTL::PixelFormatRGB9E5Float*/

//RGBA

//8
#define LV_FORMAT_R8G8B8A8_UINT 73 /*MTL::PixelFormatRGBA8Uint*/
#define LV_FORMAT_R8G8B8A8_SINT 74 /*MTL::PixelFormatRGBA8Sint*/
#define LV_FORMAT_R8G8B8A8_UNORM 70 /*MTL::PixelFormatRGBA8Unorm*/
#define LV_FORMAT_R8G8B8A8_SNORM 72 /*MTL::PixelFormatRGBA8Snorm*/
#define LV_FORMAT_R8G8B8A8_UNORM_SRGB 71 /*MTL::PixelFormatRGBA8Unorm_sRGB*/

//16
#define LV_FORMAT_R16G16B16A16_UINT 113 /*MTL::PixelFormatRGBA16Uint*/
#define LV_FORMAT_R16G16B16A16_SINT 114 /*MTL::PixelFormatRGBA16Sint*/
#define LV_FORMAT_R16G16B16A16_UNORM 110 /*MTL::PixelFormatRGBA16Unorm*/
#define LV_FORMAT_R16G16B16A16_SNORM 112 /*MTL::PixelFormatRGBA16Snorm*/
#define LV_FORMAT_R16G16B16A16_SFLOAT 115 /*MTL::PixelFormatRGBA16Float*/

//32
#define LV_FORMAT_R32G32B32A32_UINT 123 /*MTL::PixelFormatRGBA32Uint*/
#define LV_FORMAT_R32G32B32A32_SINT 124 /*MTL::PixelFormatRGBA32Sint*/
#define LV_FORMAT_R32G32B32A32_SFLOAT 125 /*MTL::PixelFormatRGBA32Float*/

//Packed
#define LV_FORMAT_A1B5G5R5_UNORM 41 /*MTL::PixelFormatA1BGR5Unorm*/
#define LV_FORMAT_B4G4R4A4_UNORM 42 /*MTL::PixelFormatABGR4Unorm*/
#define LV_FORMAT_B5G5R5A1_UNORM 43 /*MTL::PixelFormatBGR5A1Unorm*/

#define LV_FORMAT_A2B10G10R10_UNORM 94 /*MTL::PixelFormatBGR10A2Unorm*/
#define LV_FORMAT_A2R10G10B10_UNORM 90 /*MTL::PixelFormatRGB10A2Unorm*/
#define LV_FORMAT_A2R10G10B10_UINT 91 /*MTL::PixelFormatRGB10A2Uint*/

//D

//All
#define LV_FORMAT_D16_UNORM 250 /*MTLPixelFormatDepth16Unorm*/
#define LV_FORMAT_D32_SFLOAT 252 /*MTLPixelFormatDepth32Float*/

//S

//All
#define LV_FORMAT_S8_UINT 253 /*MTLPixelFormatStencil8*/

//DS

//All
#define LV_FORMAT_D24_UNORM_S8_UINT 255 /*MTLPixelFormatDepth24Unorm_Stencil8*/
#define LV_FORMAT_D32_SFLOAT_S8_UINT 260 /*MTLPixelFormatDepth32Float_Stencil8*/

//ASTC

//All

#define LV_FORMAT_ASTC_4X4_SRGB 186 /*MTLPixelFormatASTC_4x4_sRGB*/
#define LV_FORMAT_ASTC_4X4_LDR 204 /*MTLPixelFormatASTC_4x4_LDR*/

//Cull mode
#define LvCullModeFlags uint8_t /*MTLCullMode*/

#define LV_CULL_MODE_NONE 0 /*MTL::CullModeNone*/
#define LV_CULL_MODE_FRONT_BIT 1 /*MTL::CullModeFront*/
#define LV_CULL_MODE_BACK_BIT 2 /*MTL::CullModeBack*/

//Front face
#define LvFrontFace uint8_t /*MTL::Winding*/

#define LV_FRONT_FACE_COUNTER_CLOCKWISE 1 /*MTL::WindingCounterClockwise*/
#define LV_FRONT_FACE_CLOCKWISE 0 /*MTL::WindingClockwise*/

//Image usage
#define LvImageUsageFlags uint16_t /*MTL::TextureUsage*/

#define LV_IMAGE_USAGE_SAMPLED_BIT 1 /*MTL::TextureUsageShaderRead*/
#define LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT 4 /*MTL::TextureUsageRenderTarget*/
#define LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT 4 /*MTL::TextureUsageRenderTarget*/
#define LV_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT 4 /*MTL::TextureUsageRenderTarget*/
#define LV_IMAGE_USAGE_INPUT_ATTACHMENT_BIT 4 /*MTL::TextureUsageRenderTarget*/
#define LV_IMAGE_USAGE_STORAGE_BIT 2 /*MTL::TextureUsageShaderWrite*/
#define LV_IMAGE_USAGE_TRANSFER_SRC_BIT 0 /*MTL::TextureUsageUnknown*/
#define LV_IMAGE_USAGE_TRANSFER_DST_BIT 0 /*MTL::TextureUsageUnknown*/

//Filter
#define LvFilter uint8_t /*MTL::SamplerMinMagFilter*/

#define LV_FILTER_NEAREST 0 /*MTL::SamplerMinMagFilterNearest*/
#define LV_FILTER_LINEAR 1 /*MTL::SamplerMinMagFilterLinear*/

//Image view type
#define LvImageViewType uint8_t /*MTL::TextureType*/

#define LV_IMAGE_VIEW_TYPE_UNDEFINED 10
#define LV_IMAGE_VIEW_TYPE_1D 0 /*MTL::TextureType1D*/
#define LV_IMAGE_VIEW_TYPE_2D 2 /*MTL::TextureType2D*/
#define LV_IMAGE_VIEW_TYPE_3D 7 /*MTL::TextureType3D*/
#define LV_IMAGE_VIEW_TYPE_CUBE 5 /*MTL::TextureTypeCube*/
#define LV_IMAGE_VIEW_TYPE_1D_ARRAY 1 /*MTL::TextureType1DArray*/
#define LV_IMAGE_VIEW_TYPE_2D_ARRAY 3 /*MTL::TextureType2DArray*/
#define LV_IMAGE_VIEW_TYPE_CUBE_ARRAY 6 /*MTL::TextureTypeCubeArray*/

//Sampler address mode
#define LvSamplerAddressMode uint8_t /*MTL::SamplerAddressMode*/

#define LV_SAMPLER_ADDRESS_MODE_REPEAT 2 /*MTL::SamplerAddressModeRepeat*/
#define LV_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 3 /*MTL::SamplerAddressModeMirrorRepeat*/
#define LV_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 0 /*MTL::SamplerAddressModeClampToEdge*/
#define LV_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 4 /*MTL::SamplerAddressModeClampToBorderZero*/
#define LV_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE 1 /*MTL::SamplerAddressModeMirrorClampToEdge*/

//Index type
#define LvIndexType uint8_t /*MTL::IndexType*/

#define LV_INDEX_TYPE_UINT16 0 /*MTL::IndexTypeUInt16*/
#define LV_INDEX_TYPE_UINT32 1 /*MTL::IndexTypeUInt32*/

//Load Op
#define LvAttachmentLoadOp uint8_t /*MTL::LoadAction*/

#define LV_ATTACHMENT_LOAD_OP_DONT_CARE 0 /*MTL::LoadActionDontCare*/
#define LV_ATTACHMENT_LOAD_OP_CLEAR 2 /*MTL::LoadActionClear*/
#define LV_ATTACHMENT_LOAD_OP_LOAD 1 /*MTL::LoadActionLoad*/

//Store Op
#define LvAttachmentStoreOp uint8_t /*MTL::StoreAction*/

#define LV_ATTACHMENT_STORE_OP_DONT_CARE 0 /*MTL::StoreActionDontCare*/
#define LV_ATTACHMENT_STORE_OP_STORE 1 /*MTL::StoreActionStore*/

//Compare op
#define LvCompareOp uint8_t /*MTL::CompareFunction*/

#define LV_COMPARE_OP_NEVER 0 /*MTL::CompareFunctionNever*/
#define LV_COMPARE_OP_LESS 1 /*MTL::CompareFunctionLess*/
#define LV_COMPARE_OP_EQUAL 2 /*MTL::CompareFunctionEqual*/
#define LV_COMPARE_OP_LESS_OR_EQUAL 3 /*MTL::CompareFunctionLessEqual*/
#define LV_COMPARE_OP_GREATER 4 /*MTL::CompareFunctionGreater*/
#define LV_COMPARE_OP_NOT_EQUAL 5 /*MTL::CompareFunctionNotEqual*/
#define LV_COMPARE_OP_GREATER_OR_EQUAL 6 /*MTL::CompareFunctionGreaterEqual*/
#define LV_COMPARE_OP_ALWAYS 7 /*MTL::CompareFunctionAlways*/

//Vertex format
#define LvVertexFormat uint8_t /*MTL::VertexFormat*/

#define LV_VERTEX_FORMAT_R8_UINT 45 /*MTL::VertexFormatUChar*/
#define LV_VERTEX_FORMAT_RG8_UINT 1 /*MTL::VertexFormatUChar2*/
#define LV_VERTEX_FORMAT_RG32_SFLOAT 29 /*MTL::VertexFormatFloat2*/
#define LV_VERTEX_FORMAT_RGB32_SFLOAT 30 /*MTL::VertexFormatFloat3*/
#define LV_VERTEX_FORMAT_RGBA32_SFLOAT 31 /*MTL::VertexFormatFloat4*/

//Image layout
#define LvImageLayout uint8_t

#define LV_IMAGE_LAYOUT_UNDEFINED 0
#define LV_IMAGE_LAYOUT_GENERAL 1
#define LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 2
#define LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL 3
#define LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL 4
#define LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL 5
#define LV_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL 6
#define LV_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 7
#define LV_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL 8
#define LV_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL 9
#define LV_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL 10
#define LV_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL 11
#define LV_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL 12
#define LV_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL 13
#define LV_IMAGE_LAYOUT_READ_ONLY_OPTIMAL 14
#define LV_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL 15

//Image aspect
#define LvImageAspectFlags uint8_t

#define LV_IMAGE_ASPECT_COLOR_BIT 0x1
#define LV_IMAGE_ASPECT_DEPTH_BIT 0x2
#define LV_IMAGE_ASPECT_STENCIL_BIT 0x4

//Memory type
#define LvMemoryType uint8_t /*MTL::StorageMode*/

#define LV_MEMORY_TYPE_PRIVATE 2 /*MTL::StorageModePrivate*/
#define LV_MEMORY_TYPE_SHARED 0 /*MTL::StorageModeShared*/
#define LV_MEMORY_TYPE_MEMORYLESS 3 /*MTL::StorageModeMemoryless*/

//Blend Op
#define LvBlendOp uint8_t /*MTL::BlendOperation*/

#define LV_BLEND_OP_ADD 0 /*MTL::BlendOperationAdd*/
#define LV_BLEND_OP_SUBTRACT 1 /*MTL::BlendOperationSubtract*/
#define LV_BLEND_OP_REVERSE_SUBTRACT 2 /*MTL::BlendOperationReverseSubtract*/
#define LV_BLEND_OP_MIN 3 /*MTL::BlendOperationMin*/
#define LV_BLEND_OP_MAX 4 /*MTL::BlendOperationMax*/

//Blend factor
#define LvBlendFactor uint8_t /*MTL::BlendFactor*/

#define LV_BLEND_FACTOR_ZERO 0 /*MTL::BlendFactorZero*/
#define LV_BLEND_FACTOR_ONE 1 /*MTL::BlendFactorOne*/
#define LV_BLEND_FACTOR_SRC_COLOR 2 /*MTL::BlendFactorSourceColor*/
#define LV_BLEND_FACTOR_ONE_MINUS_SRC_COLOR 3 /*MTL::BlendFactorOneMinusSourceColor*/
#define LV_BLEND_FACTOR_DST_COLOR 6 /*MTL::BlendFactorDestinationColor*/
#define LV_BLEND_FACTOR_ONE_MINUS_DST_COLOR 7 /*MTL::BlendFactorOneMinusDestinationColor*/
#define LV_BLEND_FACTOR_SRC_ALPHA 4 /*MTL::BlendFactorSourceAlpha*/
#define LV_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA 5 /*MTL::BlendFactorOneMinusSourceAlpha*/
#define LV_BLEND_FACTOR_DST_ALPHA 8 /*MTL::BlendFactorDestinationAlpha*/
#define LV_BLEND_FACTOR_ONE_MINUS_DST_ALPHA 9 /*MTL::BlendFactorOneMinusDestinationAlpha*/
#define LV_BLEND_FACTOR_CONSTANT_COLOR MTL::BlendFactorZero //TODO: finish
#define LV_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR MTL::BlendFactorZero
#define LV_BLEND_FACTOR_CONSTANT_ALPHA MTL::BlendFactorZero
#define LV_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA MTL::BlendFactorZero

//Command buffer usage
#define LvCommandBufferUsageFlags uint8_t

#define LV_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 0x1

//Descriptor type
#define LvDescriptorType uint8_t

#define LV_DESCRIPTOR_TYPE_UNDEFINED 0
#define LV_DESCRIPTOR_TYPE_SAMPLER 1
#define LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 2
#define LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE 3
#define LV_DESCRIPTOR_TYPE_STORAGE_IMAGE 4
#define LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER 5
#define LV_DESCRIPTOR_TYPE_STORAGE_BUFFER 6
#define LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT 7

//Buffer usage
#define LvBufferUsageFlags uint16_t

#define LV_BUFFER_USAGE_TRANSFER_SRC_BIT 0x1
#define LV_BUFFER_USAGE_TRANSFER_DST_BIT 0x2
#define LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT 0x4
#define LV_BUFFER_USAGE_STORAGE_BUFFER_BIT 0x8
#define LV_BUFFER_USAGE_INDEX_BUFFER_BIT 0x10
#define LV_BUFFER_USAGE_VERTEX_BUFFER_BIT 0x20

//Memory allocation create
#define LvMemoryAllocationCreateFlags uint8_t

#define LV_MEMORY_ALLOCATION_CREATE_DEDICATED_BIT 0x1
#define LV_MEMORY_ALLOCATION_CREATE_MIN_MEMORY_BIT 0x2

//Command buffer usage
#define LvCommandBufferCreateFlags uint8_t

#define LV_COMMAND_BUFFER_CREATE_FENCE_TO_WAIT_UNTIL_COMPLETE_BIT 0x1

//Winding order
#define LvWindingOrder uint8_t

#define LV_WINDING_ORDER_CLOCKWISE 0 /*MTLWindingClockwise*/
#define LV_WINDING_ORDER_COUNTER_CLOCKWISE 1 /*MTLWindingCounterClockwise*/

//Tessellation partition mode
#define LvTessellationSpacing uint8_t

#define LV_TESSELLATION_SPACING_EQUAL 1 /*MTLTessellationPartitionModeInteger*/
#define LV_TESSELLATION_SPACING_FRACTIONAL_ODD 2 /*MTLTessellationPartitionModeFractionalOdd*/
#define LV_TESSELLATION_SPACING_FRACTIONAL_EVEN 3 /*MTLTessellationPartitionModeFractionalEven*/

//Vertex step function
#define LvVertexStepFunction uint8_t

#define LV_VERTEX_STEP_FUNCTION_PER_VERTEX 1 /*MTLVertexStepFunctionPerVertex*/
#define LV_VERTEX_STEP_FUNCTION_PER_PATCH 3 /*MTLVertexStepFunctionPerPatch*/
#define LV_VERTEX_STEP_FUNCTION_PER_PATCH_CONTROL_POINT 4 /*MTLVertexStepFunctionPerPatchControlPoint*/

//Clear values
union LvClearColorValue {
    float       float32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    int32_t     int32[4];
    uint32_t    uint32[4];
};

struct LvClearDepthStencilValue {
    float       depth = 1.0f;
    uint32_t    stencil = 0;
};

struct LvClearValue {
    LvClearColorValue color{};
    LvClearDepthStencilValue depthStencil{};
};

//Data type
#define LvDataType uint8_t /*MTLDataType*/

//Command encoder state
enum LvCommandEncoderState {
    LV_COMMAND_ENCODER_STATE_RENDER,
    LV_COMMAND_ENCODER_STATE_COMPUTE,
    LV_COMMAND_ENCODER_STATE_BLIT
};

#endif
