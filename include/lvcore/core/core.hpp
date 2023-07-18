#ifndef LV_CORE_H
#define LV_CORE_H

#include <stdint.h>

#define LV_SHADER_STAGE_VERTEX_INDEX 0
#define LV_SHADER_STAGE_FRAGMENT_INDEX 1
#define LV_SHADER_STAGE_COMPUTE_INDEX 2

namespace lv {

enum class Bool {
    False,
    True
};

enum class Format {
    //---------------- 8 ----------------

    //R
    R8Uint,
    R8Sint,
    R8Unorm,
    R8Snorm,
    R8Unorm_sRGB,

    //S
    S8Uint,

    //---------------- 16 ----------------

    //R
    R16Uint,
    R16Sint,
    R16Unorm,
    R16Snorm,

    //RG
    R8G8Uint,
    R8G8Sint,
    R8G8Unorm,
    R8G8Snorm,
    R8G8Unorm_sRGB,

    //D
    D16Unorm,

    //BGR - packed
    B5G6R5Unorm,

    //ABGR - packed
    A1B5G5R5Unorm,

    //BGRA - packed
    B4G4R4A4Unorm,
    B5G5R5A1Unorm,

    //---------------- 32 ----------------

    //R
    R32Uint,
    R32Sint,
    R32Float,

    //RG
    R16G16Uint,
    R16G16Sint,
    R16G16Unorm,
    R16G16Snorm,

    //RGBA
    R8G8B8A8Uint,
    R8G8B8A8Sint,
    R8G8B8A8Unorm,
    R8G8B8A8Snorm,
    R8G8B8A8Unorm_sRGB,

    //BGRA
    B8G8R8A8Unorm_sRGB,

    //D
    D32Float,

    //D_S
    D24Unorm_S8Uint,

    //BGR - packed
    B10G11R11Float,

    //ERGB - packed
    E5R9G9B9Float,

    //ABGR
    A2B10G10R10Unorm,

    //ARGB
    A2R10G10B10Unorm,
    A2R10G10B10Uint,

    //---------------- 40 ----------------

    //D_S
    D32Float_S8Uint,

    //---------------- 64 ----------------

    //RG
    R32G32Uint,
    R32G32Sint,

    //RGBA
    R16G16B16A16Uint,
    R16G16B16A16Sint,
    R16G16B16A16Unorm,
    R16G16B16A16Snorm,
    R16G16B16A16Float,

    //---------------- 128 ----------------

    //RGBA
    R32G32B32A32Uint,
    R32G32B32A32Sint,
    R32G32B32A32Float,

    //---------------- ASTC ----------------

    ASTC_4x4_sRGB,
    ASTC_4x4_LDR,

    MaxEnum
};

enum class CullMode {
    Front,
    Back,

    MaxEnum
};

enum class FrontFace {
    Clockwise,
    CounterClockwise,

    MaxEnum
};

enum class Filter {
    Nearest,
    Linear,

    MaxEnum
};

enum class ImageType {
    _1D,
    _2D,
    _3D,
    Cube,
    _1DArray,
    _2DArray,
    CubeArray,

    MaxEnum
};

enum class SamplerAddressMode {
    Repeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge,

    MaxEnum
};

enum class IndexType {
    Uint16,
    Uint32,

    MaxEnum
};

enum class AttachmentLoadOperation {
    DontCare,
    Clear,
    Load,

    MaxEnum
};

enum class AttachmentStoreOperation {
    DontCare,
    Store,

    MaxEnum
};

enum class CompareOperation {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,

    MaxEnum
};

enum class ImageLayout {
    Undefined,
    General,
    ColorAttachmentOptimal,
    DepthStencilAttachmentOptimal,
    DepthStencilReadOnlyOptimal,
    ShaderReadOnlyOptimal,
    TransferSourceOptimal,
    TransferDestinationOptimal,
    DepthReadOnlyStencilAttachmentOptimal,
    DepthAttachmentStencilReadOnlyOptimal,
    DepthAttachmentOptimal,
    DepthReadOnlyOptimal,
    StencilAttachmentOptimal,
    StencilReadOnlyOptimal,
    ReadOnlyOptimal,
    AttachmentOptimal,

    MaxEnum
};

enum class MemoryType {
    Private,
    Shared,
    Memoryless,

    MaxEnum
};

enum class BlendOperation {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,

    MaxEnum
};

enum class BlendFactor {
    Zero,
    One,
    SourceColor,
    OneMinusSourceColor,
    DestinationColor,
    OneMinusDestinationColor,
    SourceAlpha,
    OneMinusSourceAlpha,
    DestinationAlpha,
    OneMinusDestinationAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,

    MaxEnum
};

enum class DescriptorType {
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformBuffer,
    StorageBuffer,
    InputAttachment,

    MaxEnum
};

enum class TessellationSpacing {
    Equal,
    FractionalOdd,
    FractionalEven,

    MaxEnum
};

enum class VertexStepFunction {
    PerVertex,
    PerPatch,
    PerPatchControlPoint,

    MaxEnum
};

enum class CommandEncoderState {
    Render,
    Compute,
    Blit,

    MaxEnum
};

enum class ShaderStageFlags {
    None     = 0x0,

    Vertex   = 0x1,
    Fragment = 0x2,
    Compute  = 0x4
};

enum class ImageUsageFlags {
    None                   = 0x0,

    Sampled                = 0x1,
    ColorAttachment        = 0x2,
    DepthStencilAttachment = 0x4,
    TransientAttachment    = 0x8,
    InputAttachment        = 0x10,
    Storage                = 0x20,
    TransferSource         = 0x40,
    TransferDestination    = 0x80
};

enum class ImageAspectFlags {
    None    = 0x0,

    Color   = 0x1,
    Depth   = 0x2,
    Stencil = 0x4
};

enum class CommandBufferUsageFlags {
    None          = 0x0,

    OneTimeSubmit = 0x1
};

enum class BufferUsageFlags {
    None                = 0x0,

    TransferSource      = 0x1,
    TransferDestination = 0x2,
    UniformBuffer       = 0x4,
    StorageBuffer       = 0x8,
    IndexBuffer         = 0x10,
    VertexBuffer        = 0x20
};

enum class MemoryAllocationCreateFlags {
    None      = 0x0,

    Dedicated = 0x1,
    MinMemory = 0x2
};

enum class CommandBufferCreateFlags {
    None                           = 0x0,

    CreateFenceToWaitUntilComplete = 0x1
};

//Clear values
union ClearColorValue {
    float       float32[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    int32_t     int32[4];
    uint32_t    uint32[4];
};

union ClearDepthStencilValue {
    float       depth;
    uint32_t    stencil;
};

union ClearValue {
    ClearColorValue color{};
    ClearDepthStencilValue depthStencil;
};

} //namespace lv

#endif
