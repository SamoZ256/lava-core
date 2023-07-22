#ifndef LV_VULKAN_SHADER_MODULE_H
#define LV_VULKAN_SHADER_MODULE_H

#include "common.hpp"

#include "lvcore/core/core.hpp"

namespace lv {

struct Vulkan_ShaderModuleCreateInfo {
    ShaderStageFlags shaderStage;
    std::string source;
    std::vector<VkSpecializationMapEntry> specializationConstants;
    void* constantsData = nullptr;
    size_t constantsSize;
};

class Vulkan_ShaderModule {
private:
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo _stageInfo;
    VkSpecializationInfo specializationInfo{};

public:
    Vulkan_ShaderModule(Vulkan_ShaderModuleCreateInfo createInfo);

    ~Vulkan_ShaderModule();

    void compile(Vulkan_ShaderModuleCreateInfo& createInfo);

    void recompile();

    //Getters
    inline VkPipelineShaderStageCreateInfo& stageInfo() { return _stageInfo; }
};

} //namespace lv

#endif
