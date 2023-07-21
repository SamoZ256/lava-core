#ifndef LV_CORE_H
#define LV_CORE_H

#include <stdint.h>
#include <type_traits>

#define LV_SHADER_STAGE_VERTEX_INDEX 0
#define LV_SHADER_STAGE_FRAGMENT_INDEX 1
#define LV_SHADER_STAGE_COMPUTE_INDEX 2

template<class T>
constexpr std::underlying_type_t<T> toUnderlying(T e) {
   return static_cast<typename std::underlying_type<T>::type>(e);
}

template<class T>
constexpr T operator& (T l, T r) {
    typedef std::underlying_type_t<T> ut;
    return static_cast<T>(static_cast<ut>(l) & static_cast<ut>(r));
}

template<class T>
constexpr T operator| (T l, T r) {
    typedef std::underlying_type_t<T> ut;
    return static_cast<T>(static_cast<ut>(l) | static_cast<ut>(r));
}

namespace lv {

enum Bool {
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
    R16Float,

    //RG
    RG8Uint,
    RG8Sint,
    RG8Unorm,
    RG8Snorm,
    RG8Unorm_sRGB,

    //D
    D16Unorm,

    //BGR - packed
    B5G6R5Unorm,

    //ABGR - packed
    A1BGR5Unorm,

    //BGRA - packed
    BGRA4Unorm,
    BGR5A1Unorm,

    //---------------- 24 ----------------

    //RGB
    RGB8Uint,
    RGB8Sint,
    RGB8Unorm,
    RGB8Snorm,

    //---------------- 32 ----------------

    //R
    R32Uint,
    R32Sint,
    R32Float,

    //RG
    RG16Uint,
    RG16Sint,
    RG16Unorm,
    RG16Snorm,
    RG16Float,

    //RGBA
    RGBA8Uint,
    RGBA8Sint,
    RGBA8Unorm,
    RGBA8Snorm,
    RGBA8Unorm_sRGB,

    //BGRA
    BGRA8Unorm_sRGB,

    //D
    D32Float,

    //D_S
    D24Unorm_S8Uint,

    //BGR - packed
    B10GR11UFloat,

    //ABGR - packed
    A2BGR10Unorm,

    //ARGB - packed
    A2RGB10Unorm,
    A2RGB10Snorm,
    A2RGB10Uint,

    //ERGB - packed
    E5RGB9UFloat,

    //---------------- 40 ----------------

    //D_S
    D32Float_S8Uint,

    //---------------- 48 ----------------

    //RGB
    RGB16Uint,
    RGB16Sint,
    RGB16Unorm,
    RGB16Snorm,
    RGB16Float,

    //---------------- 64 ----------------

    //RG
    RG32Uint,
    RG32Sint,
    RG32Float,

    //RGBA
    RGBA16Uint,
    RGBA16Sint,
    RGBA16Unorm,
    RGBA16Snorm,
    RGBA16Float,

    //---------------- 96 ----------------

    //RGB
    RGB32Uint,
    RGB32Sint,
    RGB32Float,

    //---------------- 128 ----------------

    //RGBA
    RGBA32Uint,
    RGBA32Sint,
    RGBA32Float,

    //---------------- ASTC ----------------

    ASTC_4x4_sRGB,
    ASTC_4x4_LDR,

    MaxEnum
};

enum class CullMode {
    None,
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
    MirrorRepeat,
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
    Pow2,

    MaxEnum
};

enum class VertexInputRate {
    Constant, //TODO: Not supported yet
    PerVertex,
    PerInstance,
    PerPatch,
    PerPatchControlPoint,

    MaxEnum
};

//TODO: use this enum
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
    StorageImage           = 0x20,
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
