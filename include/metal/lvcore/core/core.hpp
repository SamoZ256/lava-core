#ifndef LV_METAL_CORE_H
#define LV_METAL_CORE_H

#import <Metal/Metal.h>

#include "lvcore/core/common.hpp"

//TODO: remove the use of 'metal' namespace from macros

namespace lv {

namespace metal {

//---------------- Pixel format ----------------
constexpr MTLPixelFormat pixelFormatLUT[] = {
    //---------------- 8 ----------------

    //R
    MTLPixelFormatR8Uint,
    MTLPixelFormatR8Sint,
    MTLPixelFormatR8Unorm,
    MTLPixelFormatR8Snorm,
    MTLPixelFormatR8Unorm_sRGB,

    //S
    MTLPixelFormatStencil8,

    //---------------- 16 ----------------

    //R
    MTLPixelFormatR16Uint,
    MTLPixelFormatR16Sint,
    MTLPixelFormatR16Unorm,
    MTLPixelFormatR16Snorm,
    MTLPixelFormatR16Float,

    //RG
    MTLPixelFormatRG8Uint,
    MTLPixelFormatRG8Sint,
    MTLPixelFormatRG8Unorm,
    MTLPixelFormatRG8Snorm,
    MTLPixelFormatRG8Unorm_sRGB,

    //D
    MTLPixelFormatDepth16Unorm,

    //BGR - packed
    MTLPixelFormatB5G6R5Unorm,

    //ABGR - packed
    MTLPixelFormatA1BGR5Unorm,

    //BGRA - packed
    MTLPixelFormatABGR4Unorm,
    MTLPixelFormatBGR5A1Unorm,

    //---------------- 24 ----------------

    //RGB
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid

    //---------------- 32 ----------------

    //R
    MTLPixelFormatR32Uint,
    MTLPixelFormatR32Sint,
    MTLPixelFormatR32Float,

    //RG
    MTLPixelFormatRG16Uint,
    MTLPixelFormatRG16Sint,
    MTLPixelFormatRG16Unorm,
    MTLPixelFormatRG16Snorm,
    MTLPixelFormatRG16Float,

    //RGBA
    MTLPixelFormatRGBA8Uint,
    MTLPixelFormatRGBA8Sint,
    MTLPixelFormatRGBA8Unorm,
    MTLPixelFormatRGBA8Snorm,
    MTLPixelFormatRGBA8Unorm_sRGB,

    //BGRA
    MTLPixelFormatBGRA8Unorm_sRGB,

    //D
    MTLPixelFormatDepth32Float,

    //D_S
    MTLPixelFormatDepth24Unorm_Stencil8,

    //BGR - packed
    MTLPixelFormatRG11B10Float,
    
    //ABGR - packed
    MTLPixelFormatBGR10A2Unorm,

    //ARGB - packed
    MTLPixelFormatRGB10A2Unorm,
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatRGB10A2Uint,

    //ERGB - packed
    MTLPixelFormatRGB9E5Float,

    //---------------- 40 ----------------

    //D_S
    MTLPixelFormatDepth32Float_Stencil8,

    //---------------- 48 ----------------

    //RGB
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid

    //---------------- 64 ----------------

    //RG
    MTLPixelFormatRG32Uint,
    MTLPixelFormatRG32Sint,
    MTLPixelFormatRG32Float,

    //RGBA
    MTLPixelFormatRGBA16Uint,
    MTLPixelFormatRGBA16Sint,
    MTLPixelFormatRGBA16Unorm,
    MTLPixelFormatRGBA16Snorm,
    MTLPixelFormatRGBA16Float,

    //---------------- 96 ----------------

    //RGB
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid
    MTLPixelFormatInvalid, //Invalid

    //---------------- 128 ----------------

    //RGBA
    MTLPixelFormatRGBA32Uint,
    MTLPixelFormatRGBA32Sint,
    MTLPixelFormatRGBA32Float,

    //---------------- ASTC ----------------

    MTLPixelFormatASTC_4x4_sRGB,
    MTLPixelFormatASTC_4x4_LDR
};

#define GET_MTL_PIXEL_FORMAT(format, retval) \
LV_CHECK_ARGUMENT(Format, format); \
retval = metal::pixelFormatLUT[(int)format];

//---------------- Vertex format ----------------
constexpr MTLVertexFormat vertexFormatLUT[] = {
    //---------------- 8 ----------------

    //R
    MTLVertexFormatUChar,
    MTLVertexFormatChar,
    MTLVertexFormatUCharNormalized,
    MTLVertexFormatCharNormalized,
    MTLVertexFormatInvalid, //Invalid

    //S
    MTLVertexFormatInvalid, //Invalid

    //---------------- 16 ----------------

    //R
    MTLVertexFormatUShort,
    MTLVertexFormatShort,
    MTLVertexFormatUShortNormalized,
    MTLVertexFormatShort,
    MTLVertexFormatHalf,

    //RG
    MTLVertexFormatUChar2,
    MTLVertexFormatChar2,
    MTLVertexFormatUChar2Normalized,
    MTLVertexFormatChar2Normalized,
    MTLVertexFormatInvalid, //Invalid

    //D
    MTLVertexFormatInvalid, //Invalid

    //BGR - packed
    MTLVertexFormatInvalid, //Invalid

    //ABGR - packed
    MTLVertexFormatInvalid, //Invalid

    //BGRA - packed
    MTLVertexFormatUChar4Normalized_BGRA,
    MTLVertexFormatInvalid, //Invalid

    //---------------- 24 ----------------

    //RGB
    MTLVertexFormatUChar3,
    MTLVertexFormatChar3,
    MTLVertexFormatUChar3Normalized,
    MTLVertexFormatChar3Normalized,

    //---------------- 32 ----------------

    //R
    MTLVertexFormatUInt,
    MTLVertexFormatInt,
    MTLVertexFormatFloat,

    //RG
    MTLVertexFormatUShort2,
    MTLVertexFormatShort2,
    MTLVertexFormatUShort2Normalized,
    MTLVertexFormatShort2Normalized,
    MTLVertexFormatHalf2,

    //RGBA
    MTLVertexFormatUChar4,
    MTLVertexFormatChar4,
    MTLVertexFormatUChar4Normalized,
    MTLVertexFormatChar4Normalized,
    MTLVertexFormatInvalid, //Invalid

    //BGRA
    MTLVertexFormatInvalid, //Invalid

    //D
    MTLVertexFormatInvalid, //Invalid

    //D_S
    MTLVertexFormatInvalid, //Invalid

    //BGR - packed
    MTLVertexFormatInvalid, //Beta (MTLVertexFormatFloatRG11B10)
    
    //ABGR - packed
    MTLVertexFormatInvalid, //Invalid

    //ARGB - packed
    MTLVertexFormatUInt1010102Normalized,
    MTLVertexFormatInt1010102Normalized,
    MTLVertexFormatInvalid, //Invalid

    //ERGB - packed
    MTLVertexFormatInvalid, //Beta (MTLVertexFormatFloatRGB9E5)

    //---------------- 40 ----------------

    //D_S
    MTLVertexFormatInvalid, //Invalid

    //---------------- 48 ----------------

    //RGB
    MTLVertexFormatUShort3,
    MTLVertexFormatShort3,
    MTLVertexFormatUShort3Normalized,
    MTLVertexFormatShort3Normalized,
    MTLVertexFormatHalf3,

    //---------------- 64 ----------------

    //RG
    MTLVertexFormatUInt2,
    MTLVertexFormatInt2,
    MTLVertexFormatFloat2,

    //RGBA
    MTLVertexFormatUShort4,
    MTLVertexFormatShort4,
    MTLVertexFormatUShort4Normalized,
    MTLVertexFormatShort4Normalized,
    MTLVertexFormatHalf4,

    //---------------- 96 ----------------

    //RGB
    MTLVertexFormatUInt3,
    MTLVertexFormatInt3,
    MTLVertexFormatFloat3,

    //---------------- 128 ----------------

    //RGBA
    MTLVertexFormatUInt4,
    MTLVertexFormatInt4,
    MTLVertexFormatFloat4,

    //---------------- ASTC ----------------

    MTLVertexFormatInvalid, //Invalid
    MTLVertexFormatInvalid  //Invalid
};

#define GET_MTL_VERTEX_FORMAT(format, retval) \
LV_CHECK_ARGUMENT(Format, format); \
retval = metal::vertexFormatLUT[(int)format];

//---------------- Cull mode ----------------
constexpr MTLCullMode cullModeLUT[] = {
    MTLCullModeNone,
    MTLCullModeFront,
    MTLCullModeBack
};

#define GET_MTL_CULL_MODE(cullMode, retval) \
LV_CHECK_ARGUMENT(CullMode, cullMode); \
retval = metal::cullModeLUT[(int)cullMode];

//---------------- Winding ----------------
constexpr MTLWinding windingLUT[] = {
    MTLWindingClockwise,
    MTLWindingCounterClockwise
};

#define GET_MTL_WINDING(frontFace, retval) \
LV_CHECK_ARGUMENT(FrontFace, frontFace); \
retval = metal::windingLUT[(int)frontFace];

//---------------- Min mag filter ----------------
constexpr MTLSamplerMinMagFilter samplerMinMagFilterLUT[] = {
    MTLSamplerMinMagFilterNearest,
    MTLSamplerMinMagFilterLinear
};

#define GET_MTL_SAMPLER_MIN_MAG_FILTER(filter, retval) \
LV_CHECK_ARGUMENT(Filter, filter); \
retval = metal::samplerMinMagFilterLUT[(int)filter];

//---------------- Texture type ----------------
constexpr MTLTextureType textureTypeLUT[] = {
    MTLTextureType1D,
    MTLTextureType2D,
    MTLTextureType3D,
    MTLTextureTypeCube,
    MTLTextureType1DArray,
    MTLTextureType2DArray,
    MTLTextureTypeCubeArray
};

#define GET_MTL_TEXTURE_TYPE(imageType, retval) \
LV_CHECK_ARGUMENT(ImageType, imageType); \
retval = metal::textureTypeLUT[(int)imageType];

//---------------- Sampler address mode ----------------
constexpr MTLSamplerAddressMode samplerAddressModeLUT[] = {
    MTLSamplerAddressModeRepeat,
    MTLSamplerAddressModeMirrorRepeat,
    MTLSamplerAddressModeClampToEdge,
    MTLSamplerAddressModeClampToBorderColor,
    MTLSamplerAddressModeMirrorClampToEdge
};

#define GET_MTL_SAMPLER_ADDRESS_MODE(samplerAddressMode, retval) \
LV_CHECK_ARGUMENT(SamplerAddressMode, samplerAddressMode); \
retval = metal::samplerAddressModeLUT[(int)samplerAddressMode];

//---------------- Index type ----------------
constexpr MTLIndexType indexTypeLUT[] = {
    MTLIndexTypeUInt16,
    MTLIndexTypeUInt32
};

#define GET_MTL_INDEX_TYPE(indexType, retval) \
LV_CHECK_ARGUMENT(IndexType, indexType); \
retval = metal::indexTypeLUT[(int)indexType];

//---------------- Load action ----------------
constexpr MTLLoadAction loadActionLUT[] = {
    MTLLoadActionDontCare,
    MTLLoadActionClear,
    MTLLoadActionLoad
};

#define GET_MTL_LOAD_ACTION(attachmentLoadOperation, retval) \
LV_CHECK_ARGUMENT(AttachmentLoadOperation, attachmentLoadOperation); \
retval = metal::loadActionLUT[(int)attachmentLoadOperation];

//---------------- Store action ----------------
constexpr MTLStoreAction storeActionLUT[] = {
    MTLStoreActionDontCare,
    MTLStoreActionStore
};

#define GET_MTL_STORE_ACTION(attachmentStoreOperation, retval) \
LV_CHECK_ARGUMENT(AttachmentStoreOperation, attachmentStoreOperation); \
retval = metal::storeActionLUT[(int)attachmentStoreOperation];

//---------------- Compare function ----------------
constexpr MTLCompareFunction compareFunctionLUT[] = {
    MTLCompareFunctionNever,
    MTLCompareFunctionLess,
    MTLCompareFunctionEqual,
    MTLCompareFunctionLessEqual,
    MTLCompareFunctionGreater,
    MTLCompareFunctionNotEqual,
    MTLCompareFunctionGreaterEqual,
    MTLCompareFunctionAlways
};

#define GET_MTL_COMPARE_FUNCTION(compareOperation, retval) \
LV_CHECK_ARGUMENT(CompareOperation, compareOperation); \
retval = metal::compareFunctionLUT[(int)compareOperation];

//---------------- Storage mode ----------------
constexpr MTLStorageMode storageModeLUT[] = {
    MTLStorageModePrivate,
    MTLStorageModeShared,
    MTLStorageModeMemoryless
};

#define GET_MTL_STORAGE_MODE(memoryType, retval) \
LV_CHECK_ARGUMENT(MemoryType, memoryType); \
retval = metal::storageModeLUT[(int)memoryType];

//---------------- Resource options ----------------
constexpr MTLResourceOptions resourceOptionsLUT[] = {
    MTLResourceStorageModePrivate,
    MTLResourceStorageModeShared,
    MTLResourceStorageModeMemoryless
};

#define GET_MTL_RESOURCE_OPTIONS(memoryType, retval) \
LV_CHECK_ARGUMENT(MemoryType, memoryType); \
retval = metal::resourceOptionsLUT[(int)memoryType];

//---------------- Blend operation ----------------
constexpr MTLBlendOperation blendOperationLUT[] = {
    MTLBlendOperationAdd,
    MTLBlendOperationSubtract,
    MTLBlendOperationReverseSubtract,
    MTLBlendOperationMin,
    MTLBlendOperationMax
};

#define GET_MTL_BLEND_OPERATION(blendOperation, retval) \
LV_CHECK_ARGUMENT(BlendOperation, blendOperation); \
retval = metal::blendOperationLUT[(int)blendOperation];

//---------------- Blend factor ----------------
constexpr MTLBlendFactor blendFactorLUT[] = {
    MTLBlendFactorZero,
    MTLBlendFactorOne,
    MTLBlendFactorSourceColor,
    MTLBlendFactorOneMinusSourceColor,
    MTLBlendFactorDestinationColor,
    MTLBlendFactorOneMinusDestinationColor,
    MTLBlendFactorSourceAlpha,
    MTLBlendFactorOneMinusSourceAlpha,
    MTLBlendFactorDestinationAlpha,
    MTLBlendFactorOneMinusDestinationAlpha,
    //TODO: check if these are the correct enums
    MTLBlendFactorBlendColor,
    MTLBlendFactorOneMinusBlendColor,
    MTLBlendFactorBlendAlpha,
    MTLBlendFactorOneMinusBlendAlpha
};

#define GET_MTL_BLEND_FACTOR(blendFactor, retval) \
LV_CHECK_ARGUMENT(BlendFactor, blendFactor); \
retval = metal::blendFactorLUT[(int)blendFactor];

//---------------- Tessellation partition mode ----------------
constexpr MTLTessellationPartitionMode tessellationPartitionModeLUT[] = {
    MTLTessellationPartitionModeInteger,
    MTLTessellationPartitionModeFractionalOdd,
    MTLTessellationPartitionModeFractionalEven,
    MTLTessellationPartitionModePow2
};

#define GET_MTL_TESSELLATION_PARTITION_MODE(tessellationSpacing, retval) \
LV_CHECK_ARGUMENT(TessellationSpacing, tessellationSpacing); \
retval = metal::tessellationPartitionModeLUT[(int)tessellationSpacing];

//---------------- Vertex step function ----------------
constexpr MTLVertexStepFunction vertexStepFunctionLUT[] = {
    MTLVertexStepFunctionConstant,
    MTLVertexStepFunctionPerVertex,
    MTLVertexStepFunctionPerInstance,
    MTLVertexStepFunctionPerPatch,
    MTLVertexStepFunctionPerPatchControlPoint
};

#define GET_MTL_VERTEX_STEP_FUNCTION(vertexInputRate, retval) \
LV_CHECK_ARGUMENT(VertexInputRate, vertexInputRate); \
retval = metal::vertexStepFunctionLUT[(int)vertexInputRate];

//---------------- Flags ----------------
MTLTextureUsage getMTLTextureUsage(ImageUsageFlags imageUsageFlags);

} //namespace metal

} //namespace lv

#endif
