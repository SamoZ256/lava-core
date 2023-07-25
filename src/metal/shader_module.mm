#include "lvcore/metal/shader_module.hpp"

#include <json/json.h>

#include "lvcore/metal/device.hpp"
#include "lvcore/metal/swap_chain.hpp"

#include <iostream>

namespace lv {

namespace metal {

#define _LV_CREATE_FUNCTION \
NSError* error; \
_function = [library newFunctionWithDescriptor:functionDescriptor \
                                         error:&error]; \
if (!_function) { \
    throw std::runtime_error("Failed to create shader module: " + std::string([[error localizedDescription] UTF8String])); \
}

ShaderModule::ShaderModule(internal::ShaderModuleCreateInfo createInfo) {
    std::string source = createInfo.source;
    source.erase(0, createInfo.source.find("section.metallib") + 16 /*"section.metallib".size()*/);

    dispatch_data_t sourceData = dispatch_data_create((void*)source.c_str(), source.size() * sizeof(char), nullptr, nullptr);

    NSError* error = nullptr;
    library = [g_metal_device->device() newLibraryWithData:sourceData
                                                   error:&error];
    //g_metal_device->device()->newLibrary()
    if (!library) {
        throw std::runtime_error([[error localizedDescription] UTF8String]);
    }

    compile(createInfo);

    //Shader resources
    std::string jsonSource = createInfo.source;
    jsonSource.erase(jsonSource.find("section.spv"));
    nlohmann::json JSON = nlohmann::json::parse(jsonSource);

    if (JSON.contains("pushConstant")) {
        pushConstantBinding = JSON["pushConstant"]["bufferBinding"];
    }

    std::string setCountStr = JSON["setCount"];
    descriptorSetCount = stoi(setCountStr);

    if (descriptorSetCount) {
        size_t size = sizeof(uint32_t*) * descriptorSetCount;
        bufferBindings = (uint32_t**)malloc(size);
        textureBindings = (uint32_t**)malloc(size);
        samplerBindings = (uint32_t**)malloc(size);

        for (auto& descriptorSet : JSON["descriptorSets"].items()) {
            auto& jsonDescriptorSet = JSON["descriptorSets"][descriptorSet.key()];
            std::string setNumberStr = descriptorSet.key();
            uint32_t setNumber = stoi(setNumberStr);
            std::string bindingCountStr = jsonDescriptorSet["bindingCount"];
            int32_t bindingCount = stoi(bindingCountStr);

            size = sizeof(uint32_t) * bindingCount;
            bufferBindings[setNumber] = (uint32_t*)malloc(size);
            textureBindings[setNumber] = (uint32_t*)malloc(size);
            samplerBindings[setNumber] = (uint32_t*)malloc(size);

            for (auto& binding : jsonDescriptorSet["bindings"].items()) {
                auto& jsonBinding = jsonDescriptorSet["bindings"][binding.key()];
                std::string bindingNumberStr = binding.key();
                uint32_t bindingNumber = stoi(bindingNumberStr);
                //std::cout << "Binding number: " << bindingNumber << std::endl;

                std::string descriptorType = jsonBinding["descriptorType"];
                if (descriptorType == "buffer") {
                    //std::cout << "Uniform buffer" << std::endl;
                    bufferBindings[setNumber][bindingNumber] = jsonBinding["bufferBinding"];
                } else if (descriptorType == "combinedImageSampler") {
                    //std::cout << "Combined image sampler" << std::endl;
                    textureBindings[setNumber][bindingNumber] = jsonBinding["textureBinding"];
                    samplerBindings[setNumber][bindingNumber] = jsonBinding["samplerBinding"];
                } else if (descriptorType == "image") {
                    textureBindings[setNumber][bindingNumber] = jsonBinding["textureBinding"];
                }
            }
        }
    }
}

ShaderModule::~ShaderModule() {
    [library release];
    [_function release];

    //Shader resources
    if (descriptorSetCount) {
        for (uint32_t i = 0; i < descriptorSetCount; i++) {
            free(bufferBindings[i]);
            free(textureBindings[i]);
            free(samplerBindings[i]);
        }
        free(bufferBindings);
        free(textureBindings);
        free(samplerBindings);
    }
}

void ShaderModule::compile(internal::ShaderModuleCreateInfo& createInfo) {
    if (createInfo.specializationConstants.size() == 0) {
        _function = [library newFunctionWithName:@"main0"];
    } else {
        MTLFunctionDescriptor* functionDescriptor = [[MTLFunctionDescriptor alloc] init];
        [functionDescriptor setName:@"main0"];

        if (createInfo.specializationConstants.size() > 0) {
            MTLFunctionConstantValues* constantValues = [[MTLFunctionConstantValues alloc] init];
            //char* constantsData = (char*)createInfo.constantsData;
            for (auto& mapEntry : createInfo.specializationConstants) {
                [constantValues setConstantValue:/*((void**)createInfo.constantsData)[mapEntry.offset]*/(char*)createInfo.constantsData + mapEntry.offset
                                            type:(MTLDataType)mapEntry.dataType
                                         atIndex:mapEntry.constantID];
            }

            functionDescriptor.constantValues = constantValues;
        }

        //function = library->newFunction(NS::String::string(createInfo.functionName, NS::StringEncoding::UTF8StringEncoding));
        _LV_CREATE_FUNCTION;
    }
}

void ShaderModule::recompile() {
    [_function release];
    throw std::runtime_error("Not implemented yet");
    //_LV_CREATE_FUNCTION;
}

} //namespace metal

} //namespace lv
