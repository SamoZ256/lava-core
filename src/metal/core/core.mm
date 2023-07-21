#include "metal/lvcore/core/core.hpp"

namespace lv {

namespace metal {

//---------------- Texture usage ----------------
MTLTextureUsage getMTLTextureUsage(ImageUsageFlags imageUsageFlags) {
    MTLTextureUsage mtlTextureUsage = 0;

    if ((int)imageUsageFlags & (int)ImageUsageFlags::Sampled)
        mtlTextureUsage |= MTLTextureUsageShaderRead;
    if ((int)imageUsageFlags & (int)ImageUsageFlags::ColorAttachment || (int)imageUsageFlags & (int)ImageUsageFlags::DepthStencilAttachment || (int)imageUsageFlags & (int)ImageUsageFlags::TransientAttachment || (int)imageUsageFlags & (int)ImageUsageFlags::InputAttachment)
        mtlTextureUsage |= MTLTextureUsageRenderTarget;
    if ((int)imageUsageFlags & (int)ImageUsageFlags::StorageImage)
        mtlTextureUsage |= MTLTextureUsageShaderWrite;
    
    return mtlTextureUsage;
}

} //namespace metal

} //namespace lv
