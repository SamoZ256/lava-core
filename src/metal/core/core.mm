#include "metal/lvcore/core/core.hpp"

namespace lv {

namespace metal {

//---------------- Texture usage ----------------
MTLTextureUsage getMTLTextureUsageFlags(ImageUsageFlags imageUsageFlags) {
    MTLTextureUsage mtlTextureUsage = 0;

    if (imageUsageFlags & ImageUsageFlags::Sampled)
        mtlTextureUsage |= MTLTextureUsageShaderRead;
    if (imageUsageFlags & ImageUsageFlags::ColorAttachment || imageUsageFlags & ImageUsageFlags::DepthStencilAttachment || imageUsageFlags & ImageUsageFlags::TransientAttachment || imageUsageFlags & ImageUsageFlags::InputAttachment)
        mtlTextureUsage |= MTLTextureUsageRenderTarget;
    if (imageUsageFlags & ImageUsageFlags::StorageImage)
        mtlTextureUsage |= MTLTextureUsageShaderWrite;
    
    return mtlTextureUsage;
}

} //namespace metal

} //namespace lv
