#ifndef LV_INTERNAL_SHADER_MODULE_H
#define LV_INTERNAL_SHADER_MODULE_H

#include "common.hpp"

namespace lv {

namespace internal {

struct SpecializationMapEntry {
    uint32_t constantID;
    uint32_t offset;
    size_t size;
    int dataType; //TODO: create an enum for this
};

struct ShaderModuleCreateInfo {
    ShaderStageFlags shaderStage;
    std::string source;
    std::vector<SpecializationMapEntry> specializationConstants;
    void* constantsData = nullptr;
    size_t constantsSize;
};

class ShaderModule {
public:
    virtual ~ShaderModule() {}

    virtual void recompile() = 0;
};

} //namespace internal

} //namespace lv

#endif
