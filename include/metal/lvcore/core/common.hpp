#ifndef LV_METAL_COMMON_H
#define LV_METAL_COMMON_H

#include <stdlib.h>
#include <vector>

#ifdef __OBJC__

#import <Metal/Metal.h>
#import <MetalFX/MetalFX.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>

#define _LV_MTL_RENDER_PIPELINE_DESCRIPTOR_T MTLRenderPipelineDescriptor*

#define _LV_MTLFX_TEMPORAL_SCALER_T id<MTLFXTemporalScaler>
#define _LV_MPS_SVGF_DENOISER_T MPSSVGFDenoiser*
#define _LV_MPS_TEMPORAL_AA_T MPSTemporalAA*

#else

typedef void* id;

#define _LV_MTL_RENDER_PIPELINE_DESCRIPTOR_T id

#define _LV_MTLFX_TEMPORAL_SCALER_T id
#define _LV_MPS_SVGF_DENOISER_T id
#define _LV_MPS_TEMPORAL_AA_T id

#endif

#define LV_METAL_VERTEX_BUFFER_BINDING_INDEX 30

size_t roundToMultipleOf16(size_t size);

#define MSL_SIZEOF(type) roundToMultipleOf16(sizeof(type))

#endif
