#ifndef LV_VULKAN_SHADER_MODULE_H
#define LV_VULKAN_SHADER_MODULE_H

#include "common.hpp"

#include "lvcore/internal/core.hpp"

namespace lv {

namespace vulkan {

struct ShaderModuleCreateInfo {
    ShaderStageFlags shaderStage;
    std::string source;
    std::vector<VkSpecializationMapEntry> specializationConstants;
    void* constantsData = nullptr;
    size_t constantsSize;
};

class ShaderModule {
private:
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo _stageInfo;
    VkSpecializationInfo specializationInfo{};

public:
    ShaderModule(ShaderModuleCreateInfo createInfo);

    ~ShaderModule();

    void compile(ShaderModuleCreateInfo& createInfo);

    void recompile();

    //Getters
    inline VkPipelineShaderStageCreateInfo& stageInfo() { return _stageInfo; }
};

} //namespace vulkan

} //namespace lv

#endif
