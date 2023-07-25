#include "lvcore/vulkan/shader_module.hpp"

#include "lvcore/vulkan/core.hpp"

#include "lvcore/vulkan/device.hpp"

namespace lv {

namespace vulkan {

ShaderModule::ShaderModule(internal::ShaderModuleCreateInfo createInfo) {
    compile(createInfo);
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(g_vulkan_device->device(), shaderModule, nullptr);
}

void ShaderModule::compile(internal::ShaderModuleCreateInfo& createInfo) {
    size_t begin = createInfo.source.find("section.spv") + 11 /*"section.spv".size()*/;
    std::string source = createInfo.source.substr(begin, createInfo.source.find("section.metallib") - begin);

    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = source.size() * sizeof(char);
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(source.data());

    VK_CHECK_RESULT(vkCreateShaderModule(g_vulkan_device->device(), &moduleCreateInfo, nullptr, &shaderModule));

    //Specialization constants
    specializationInfo.mapEntryCount = createInfo.specializationConstants.size();
    specializationInfo.pMapEntries = (VkSpecializationMapEntry*)createInfo.specializationConstants.data();
    specializationInfo.dataSize = createInfo.constantsSize;
    specializationInfo.pData = createInfo.constantsData;

    _stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _stageInfo.stage = getVKShaderStageFlagBits(createInfo.shaderStage);
    _stageInfo.module = shaderModule;
    _stageInfo.pName = "main";
    _stageInfo.flags = 0;
    _stageInfo.pNext = nullptr;
    _stageInfo.pSpecializationInfo = (createInfo.specializationConstants.size() == 0 ? nullptr : &specializationInfo);
}

void ShaderModule::recompile() {
    vkDestroyShaderModule(g_vulkan_device->device(), shaderModule, nullptr);
    throw std::runtime_error("Not implemented yet");
    //compile();
}

} //namespace vulkan

} //namespace lv
