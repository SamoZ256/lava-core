#ifndef LV_METAL_CORE_H
#define LV_METAL_CORE_H

#import <Metal/Metal.h>

#include "lvcore/core/core.hpp"

namespace lv {

//---------------- Pixel format ----------------
constexpr MTLPixelFormat pixelFormatLUT[] = {
    MTLPixelFormatR8Uint,
    MTLPixelFormatR8Sint,
    MTLPixelFormatR8Unorm,
    MTLPixelFormatR8Snorm,
    MTLPixelFormatR8Unorm_sRGB,

    MTLPixelFormatStencil8,

    MTLPixelFormatR16Uint,
    MTLPixelFormatR16Sint,
    MTLPixelFormatR16Unorm,
    MTLPixelFormatR16Snorm,

    MTLPixelFormatRG8Uint,
    MTLPixelFormatRG8Sint,
    MTLPixelFormatRG8Unorm,
    MTLPixelFormatRG8Snorm,
    MTLPixelFormatRG8Unorm_sRGB,

    MTLPixelFormatDepth16Unorm,

    MTLPixelFormatB5G6R5Unorm,

    MTLPixelFormatA1BGR5Unorm,

    MTLPixelFormatABGR4Unorm,
    MTLPixelFormatBGR5A1Unorm,

    MTLPixelFormatR32Uint,
    MTLPixelFormatR32Sint,
    MTLPixelFormatR32Float,

    MTLPixelFormatRG16Uint,
    MTLPixelFormatRG16Sint,
    MTLPixelFormatRG16Unorm,
    MTLPixelFormatRG16Snorm,

    MTLPixelFormatRGBA8Uint,
    MTLPixelFormatRGBA8Sint,
    MTLPixelFormatRGBA8Unorm,
    MTLPixelFormatRGBA8Snorm,
    MTLPixelFormatRGBA8Unorm_sRGB,

    MTLPixelFormatBGRA8Unorm_sRGB,

    MTLPixelFormatDepth32Float,

    MTLPixelFormatDepth24Unorm_Stencil8,

    MTLPixelFormatRG11B10Float,

    MTLPixelFormatRGB9E5Float,
    
    MTLPixelFormatBGR10A2Unorm,

    MTLPixelFormatRGB10A2Unorm,
    MTLPixelFormatRGB10A2Uint,

    MTLPixelFormatDepth32Float_Stencil8,

    MTLPixelFormatRG32Uint,
    MTLPixelFormatRG32Sint,

    MTLPixelFormatRGBA16Uint,
    MTLPixelFormatRGBA16Sint,
    MTLPixelFormatRGBA16Unorm,
    MTLPixelFormatRGBA16Snorm,
    MTLPixelFormatRGBA16Float,

    MTLPixelFormatRGBA32Uint,
    MTLPixelFormatRGBA32Sint,
    MTLPixelFormatRGBA32Float,

    MTLPixelFormatASTC_4x4_sRGB,
    MTLPixelFormatASTC_4x4_LDR
};

#define GET_MTL_PIXEL_FORMAT(format, retval) \
LV_CHECK_ARGUMENT(Format, format); \
retval = pixelFormatLUT[(int)format];

//---------------- Cull mode ----------------
constexpr MTLCullMode cullModeLUT[] = {
    MTLCullModeFront,
    MTLCullModeBack
};

#define GET_MTL_CULL_MODE(cullMode) \
LV_CHECK_ARGUMENT(CullMode, cullMode); \
MTLCullMode mtl_##cullMode = cullModeLUT[format];

//---------------- Winding ----------------
constexpr MTLWinding windingOrderLUT[] = {
    MTLWindingClockwise,
    MTLWindingCounterClockwise
};

#define GET_MTL_WINDING_ORDER(frontFace) \
LV_CHECK_ARGUMENT(FrontFace, frontFace); \
MTLWindingOrder mtl_##frontFace = windingOrderLUT[frontFace];

//---------------- Min mag filter ----------------
constexpr MTLSamplerMinMagFilter samplerMinMagFilterLUT[] = {
    MTLSamplerMinMagFilterNearest,
    MTLSamplerMinMagFilterLinear
};

#define GET_MTL_SAMPLER_MIN_MAG_FILTER(filter) \
LV_CHECK_ARGUMENT(Filter, filter); \
MTLSamplerMinMagFilter mtl_##filter = samplerMinMagFilterLUT[filter];

//---------------- Image type ----------------
constexpr MTLTextureType textureTypeLUT[] = {
    MTLTextureType1D,
    MTLTextureType2D,
    MTLTextureType3D,
    MTLTextureTypeCube,
    MTLTextureType1DArray,
    MTLTextureType2DArray,
    MTLTextureTypeCubeArray
};

#define GET_MTL_TEXTURE_TYPE(imageType) \
LV_CHECK_ARGUMENT(ImageType, imageType); \
MTLTextureType mtl_##imageType = textureTypeLUT[imageType];

//---------------- Sampler address mode ----------------
constexpr MTLSamplerAddressMode samplerAddressModeLUT[] = {
    MTLSamplerAddressModeRepeat,
    MTLSamplerAddressModeClampToEdge,
    MTLSamplerAddressModeClampToBorderColor,
    MTLSamplerAddressModeMirrorClampToEdge
};

#define GET_MTL_SAMPLER_ADDRESS_MODE(samplerAddressMode) \
LV_CHECK_ARGUMENT(SamplerAddressMode, samplerAddressMode); \
MTLSamplerAddressMode mtl_##samplerAddresMode = samplerAddressModeLUT[samplerAddressMode];

//---------------- Flags ----------------
MTLTextureUsage getMTLTextureUsage(ImageUsageFlags imageUsageFlags);

} //namespace lv

#endif
