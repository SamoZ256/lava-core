#include "vulkan/lvcore/core/shader_module.hpp"

#include "vulkan/lvcore/core/device.hpp"

namespace lv {

Vulkan_ShaderModule::Vulkan_ShaderModule(Vulkan_ShaderModuleCreateInfo createInfo) {
    compile(createInfo);
}

Vulkan_ShaderModule::~Vulkan_ShaderModule() {
    vkDestroyShaderModule(g_vulkan_device->device(), shaderModule, nullptr);
}

void Vulkan_ShaderModule::compile(Vulkan_ShaderModuleCreateInfo& createInfo) {
    size_t begin = createInfo.source.find("section.spv") + 11 /*"section.spv".size()*/;
    std::string source = createInfo.source.substr(begin, createInfo.source.find("section.metallib") - begin);

    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = source.size() * sizeof(char);
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(source.data());

    VK_CHECK_RESULT(vkCreateShaderModule(g_vulkan_device->device(), &moduleCreateInfo, nullptr, &shaderModule));

    //Specialization constants
    specializationInfo.mapEntryCount = createInfo.specializationConstants.size();
    specializationInfo.pMapEntries = createInfo.specializationConstants.data();
    specializationInfo.dataSize = createInfo.constantsSize;
    specializationInfo.pData = createInfo.constantsData;
    //std::cout << "Size: " << constants.size() << " : " << (constants.size() == 0 ? "none" : std::to_string((int)constants[0].constantID) + ", " + std::to_string(*(float*)constantsData) + ", " + std::to_string(constants[0].size)) << std::endl;

    _stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _stageInfo.stage = createInfo.shaderStage;
    _stageInfo.module = shaderModule;
    _stageInfo.pName = "main";
    _stageInfo.flags = 0;
    _stageInfo.pNext = nullptr;
    _stageInfo.pSpecializationInfo = (createInfo.specializationConstants.size() == 0 ? nullptr : &specializationInfo);
}

void Vulkan_ShaderModule::recompile() {
    vkDestroyShaderModule(g_vulkan_device->device(), shaderModule, nullptr);
    throw std::runtime_error("Not implemented yet");
    //compile();
}

} //namespace lv
