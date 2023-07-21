#ifndef LV_METAL_SHADER_MODULE_H
#define LV_METAL_SHADER_MODULE_H

#include <string>
#include <vector>

#include "lvcore/core/core.hpp"

#include "common.hpp"

namespace lv {

struct Metal_SpecializationMapEntry {
    uint32_t constantID;
    uint32_t offset;
    size_t size;
    int dataType; //TODO: create an enum for this
};

struct Metal_ShaderModuleCreateInfo {
    ShaderStageFlags shaderStage;
    std::string source;
    std::vector<Metal_SpecializationMapEntry> specializationConstants;
    void* constantsData = nullptr;
    size_t constantsSize;
};

class Metal_ShaderModule {
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

    Metal_ShaderModule(Metal_ShaderModuleCreateInfo createInfo);

    ~Metal_ShaderModule();

    void compile(Metal_ShaderModuleCreateInfo& createInfo);

    void recompile();

    //Getters
    inline id /*MTLFunction*/ function() { return _function; }
};

} //namespace lv

#endif
