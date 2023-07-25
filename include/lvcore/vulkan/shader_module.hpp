#ifndef LV_VULKAN_SHADER_MODULE_H
#define LV_VULKAN_SHADER_MODULE_H

#include "lvcore/internal/shader_module.hpp"

#include "common.hpp"

namespace lv {

namespace vulkan {

class ShaderModule : public internal::ShaderModule {
private:
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo _stageInfo;
    VkSpecializationInfo specializationInfo{};

public:
    ShaderModule(internal::ShaderModuleCreateInfo createInfo);

    ~ShaderModule() override;

    void compile(internal::ShaderModuleCreateInfo& createInfo);

    void recompile() override;

    //Getters
    inline VkPipelineShaderStageCreateInfo& stageInfo() { return _stageInfo; }
};

} //namespace vulkan

} //namespace lv

#endif
