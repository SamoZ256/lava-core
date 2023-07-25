#ifndef LV_METAL_SHADER_MODULE_H
#define LV_METAL_SHADER_MODULE_H

#include "lvcore/internal/shader_module.hpp"

#include "common.hpp"

namespace lv {

namespace metal {

class ShaderModule : public internal::ShaderModule {
private:
    id /*MTLLibrary*/ library;
    id /*MTLFunction*/ _function;

public:
    //Shader resources
    uint32_t** bufferBindings;
    uint32_t** textureBindings;
    uint32_t** samplerBindings;
    uint32_t descriptorSetCount;

    uint32_t pushConstantBinding;

    ShaderModule(internal::ShaderModuleCreateInfo createInfo);

    ~ShaderModule() override;

    void compile(internal::ShaderModuleCreateInfo& createInfo);

    void recompile() override;

    //Getters
    inline id /*MTLFunction*/ function() { return _function; }
};

} //namespace metal

} //namespace lv

#endif
