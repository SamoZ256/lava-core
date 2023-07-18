#include "lvcore/threading/thread_pool.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "lvcore/core/instance.hpp"
#include "lvcore/core/device.hpp"
#include "lvcore/core/swap_chain.hpp"
#include "lvcore/core/pipeline_layout.hpp"
#include "lvcore/core/shader_module.hpp"
#include "lvcore/core/graphics_pipeline.hpp"
#include "lvcore/core/vertex_descriptor.hpp"
#include "lvcore/core/buffer.hpp"
#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/compute_pipeline.hpp"
#include "lvcore/core/command_buffer.hpp"

#include "glm.hpp"

#include "application.hpp"

#define OBJECT_X_COUNT 16
#define OBJECT_Y_COUNT 16
#define OBJECT_COUNT (OBJECT_X_COUNT * OBJECT_Y_COUNT)

struct MainVertex {
    glm::vec3 position;
    glm::vec3 color;
};

MainVertex vertices[4] = {
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}
};

uint16_t indices[6] = {
    0, 1, 2,
    0, 2, 3
};

struct Transform {
    glm::vec4 position; //HACK: for some reason GLM_FORCE_DEFAULT_ALIGNED_GENTYPES does not work
    glm::vec4 rotation;
    glm::vec4 scale;
};

Transform transforms[OBJECT_COUNT];

class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool threadPool;
    lv::Instance instance;
    lv::Device device;
    lv::SwapChain swapChain;

    lv::CommandBuffer modelCommandBuffer;

    lv::PipelineLayout modelPipelineLayout;
    lv::PipelineLayout mainPipelineLayout;

    lv::ShaderModule compModelShaderModule;
    lv::ComputePipeline modelComputePipeline;

    lv::ShaderModule vertMainShaderModule;
    lv::ShaderModule fragMainShaderModule;
    lv::GraphicsPipeline mainGraphicsPipeline;

    lv::VertexDescriptor mainVertexDescriptor = lv::VertexDescriptor(sizeof(MainVertex));

    lv::Buffer vertexBuffer;
    lv::Buffer indexBuffer;
    lv::Buffer modelUniformBuffer;
    lv::Buffer mainUniformBuffer;
    lv::Buffer storageSpaceBuffer;

    lv::DescriptorSet modelDescriptorSet;
    lv::DescriptorSet mainDescriptorSet;

    const float nearPlane = 0.01f;
    const float farPlane = 100.0f;
    const float fov = 45.0f;
    float aspectRatio;

    glm::mat4 viewProj;

    float prevTime = 0.0f;

    LavaCoreExampleApp() : Application("compute_shader") {
        lv::ThreadPoolCreateInfo threadPoolCreateInfo;
        threadPool.init(threadPoolCreateInfo);

        lv::InstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.applicationName = "Lava Engine";
        instanceCreateInfo.validationEnable = true;
        instance.init(instanceCreateInfo);

        lv::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.window = window;
        deviceCreateInfo.threadPool = &threadPool;
        device.init(deviceCreateInfo);

        lv::SwapChainCreateInfo swapChainCreateInfo;
        swapChainCreateInfo.window = window;
        swapChainCreateInfo.vsyncEnabled = true;
        swapChainCreateInfo.maxFramesInFlight = 3;
        swapChainCreateInfo.clearAttachment = true;
        swapChain.init(swapChainCreateInfo);

        //CommandBuffer
        modelCommandBuffer.init();

        //Vertex descriptor
        mainVertexDescriptor.addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));
        mainVertexDescriptor.addBinding(1, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, color));

        //Pipeline layout

        //Model
        modelPipelineLayout.descriptorSetLayouts.resize(1);
        modelPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_STORAGE_BUFFER, LV_SHADER_STAGE_COMPUTE_BIT);
        modelPipelineLayout.descriptorSetLayouts[0].addBinding(1, LV_DESCRIPTOR_TYPE_STORAGE_BUFFER, LV_SHADER_STAGE_COMPUTE_BIT);

        modelPipelineLayout.init();

        //Main
        mainPipelineLayout.pushConstantRanges.resize(1);
        mainPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        mainPipelineLayout.pushConstantRanges[0].offset = 0;
        mainPipelineLayout.pushConstantRanges[0].size = sizeof(uint32_t);

        mainPipelineLayout.descriptorSetLayouts.resize(1);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(1, LV_DESCRIPTOR_TYPE_STORAGE_BUFFER, LV_SHADER_STAGE_VERTEX_BIT);

        mainPipelineLayout.init();

        //Compute pipeline
        lv::ShaderModuleCreateInfo compModelShaderModuleCI;
        compModelShaderModuleCI.shaderStage = LV_SHADER_STAGE_COMPUTE_BIT;
        compModelShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/compute/compute_model.json").c_str());
        compModelShaderModule.init(compModelShaderModuleCI);

        lv::ComputePipelineCreateInfo modelComputePipelineCI;
        modelComputePipelineCI.computeShaderModule = &compModelShaderModule;
        modelComputePipelineCI.pipelineLayout = &modelPipelineLayout;
        modelComputePipeline.init(modelComputePipelineCI);

        //Graphics pipeline
        lv::ShaderModuleCreateInfo vertMainShaderModuleCI;
        vertMainShaderModuleCI.shaderStage = LV_SHADER_STAGE_VERTEX_BIT;
        vertMainShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/vertex/main.json").c_str());
        vertMainShaderModule.init(vertMainShaderModuleCI);

        lv::ShaderModuleCreateInfo fragMainShaderModuleCI;
        fragMainShaderModuleCI.shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT;
        fragMainShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/fragment/main.json").c_str());
        fragMainShaderModule.init(fragMainShaderModuleCI);

        mainGraphicsPipeline.vertexShaderModule = &vertMainShaderModule;
        mainGraphicsPipeline.fragmentShaderModule = &fragMainShaderModule;
        mainGraphicsPipeline.pipelineLayout = &mainPipelineLayout;
        mainGraphicsPipeline.renderPass = &swapChain.renderPass;
        mainGraphicsPipeline.vertexDescriptor = &mainVertexDescriptor;

        mainGraphicsPipeline.addColorBlendAttachment({0});

        mainGraphicsPipeline.init();

        //Vertex buffer
        vertexBuffer.frameCount = 1;
        vertexBuffer.usage = LV_BUFFER_USAGE_VERTEX_BUFFER_BIT | LV_BUFFER_USAGE_TRANSFER_DST_BIT;
        vertexBuffer.init(sizeof(vertices));
        vertexBuffer.copyDataTo(0, vertices);

        //Index buffer
        indexBuffer.frameCount = 1;
        indexBuffer.usage = LV_BUFFER_USAGE_INDEX_BUFFER_BIT | LV_BUFFER_USAGE_TRANSFER_DST_BIT;
        indexBuffer.init(sizeof(indices));
        indexBuffer.copyDataTo(0, indices);

        //Uniform buffer
        modelUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        modelUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        modelUniformBuffer.init(sizeof(transforms));
        
        mainUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mainUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        mainUniformBuffer.init(sizeof(glm::mat4));

        //Storage space buffer
        storageSpaceBuffer.usage = LV_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        storageSpaceBuffer.init(sizeof(glm::mat4) * OBJECT_COUNT);

        //Descriptor set
        modelDescriptorSet.pipelineLayout = &modelPipelineLayout;
        modelDescriptorSet.layoutIndex = 0;
        modelDescriptorSet.addBinding(modelUniformBuffer.descriptorInfo(LV_DESCRIPTOR_TYPE_STORAGE_BUFFER), 0);
        modelDescriptorSet.addBinding(storageSpaceBuffer.descriptorInfo(LV_DESCRIPTOR_TYPE_STORAGE_BUFFER), 1);
        modelDescriptorSet.init();

        mainDescriptorSet.pipelineLayout = &mainPipelineLayout;
        mainDescriptorSet.layoutIndex = 0;
        mainDescriptorSet.addBinding(mainUniformBuffer.descriptorInfo(), 0);
        mainDescriptorSet.addBinding(storageSpaceBuffer.descriptorInfo(LV_DESCRIPTOR_TYPE_STORAGE_BUFFER), 1);
        mainDescriptorSet.init();

        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        aspectRatio = (float)width / (float)height;

        glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -48.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        viewProj = projection * view;

        //Initialize transforms
        for (uint8_t y = 0; y < OBJECT_Y_COUNT; y++) {
            for (uint8_t x = 0; x < OBJECT_X_COUNT; x++) {
                transforms[y * OBJECT_X_COUNT + x].position = glm::vec4((x - OBJECT_X_COUNT / 2) * 2, (y - OBJECT_Y_COUNT / 2) * 2, 0.0f, 0.0f);
#define RANDOM_ROTATION ((rand() % 1000) / 1000.0f * 360.0f)
                transforms[y * OBJECT_X_COUNT + x].rotation = glm::radians(glm::vec4(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION, 0.0f));
#undef RANDOM_ROTATION
#define RANDOM_SCALE ((rand() % 1000) / 1000.0f * 2.0f)
                transforms[y * OBJECT_X_COUNT + x].scale = glm::vec4(RANDOM_SCALE, RANDOM_SCALE, 0.0f, 0.0f);
#undef RANDOM_SCALE
            }
        }
    }

    ~LavaCoreExampleApp() {
        modelDescriptorSet.destroy();
        mainDescriptorSet.destroy();

        storageSpaceBuffer.destroy();
        mainUniformBuffer.destroy();
        modelUniformBuffer.destroy();
        indexBuffer.destroy();
        vertexBuffer.destroy();

        mainGraphicsPipeline.destroy();
        vertMainShaderModule.destroy();
        fragMainShaderModule.destroy();

        compModelShaderModule.destroy();
        modelComputePipeline.destroy();

        mainPipelineLayout.destroy();
        modelPipelineLayout.destroy();

        swapChain.destroy();
        device.destroy();
        instance.destroy();
    }

    void update() override {
        float crntTime = lvndGetTime();
        float dt = crntTime - prevTime;
        prevTime = crntTime;

        swapChain.acquireNextImage();

        //Computing models
        modelCommandBuffer.bind();

        modelComputePipeline.bind();

        float rotationVal = glm::radians(60.0f * dt);
        for (uint8_t y = 0; y < OBJECT_Y_COUNT; y++) {
            for (uint8_t x = 0; x < OBJECT_X_COUNT; x++) {
                transforms[y * OBJECT_X_COUNT + x].rotation.x += rotationVal;
                transforms[y * OBJECT_X_COUNT + x].rotation.y += rotationVal;
                transforms[y * OBJECT_X_COUNT + x].rotation.z += rotationVal;
            }
        }

        modelUniformBuffer.copyDataTo(0, transforms);

        modelDescriptorSet.bind();

        const uint8_t threadsPerGroup = 16;
        modelComputePipeline.dispatch(OBJECT_X_COUNT / threadsPerGroup, OBJECT_Y_COUNT / threadsPerGroup, 1, threadsPerGroup, threadsPerGroup, 1);

        modelCommandBuffer.unbind();
        modelCommandBuffer.submit();

        //Rendering
        swapChain.commandBuffer.bind();
        swapChain.framebuffer.bind();

        mainGraphicsPipeline.bind();

        mainUniformBuffer.copyDataTo(0, &viewProj);

        mainDescriptorSet.bind();

        vertexBuffer.bindVertexBuffer();
        indexBuffer.bindIndexBuffer(LV_INDEX_TYPE_UINT16);

        //glm::mat4 model = glm::mat4(1.0f);
        //storageSpaceBuffer.copyDataTo(0, &model, sizeof(glm::mat4));

        for (uint32_t i = 0; i < OBJECT_COUNT; i++) {
            mainGraphicsPipeline.uploadPushConstants(&i, 0);

            indexBuffer.drawIndexed(sizeof(uint16_t));
        }

        swapChain.framebuffer.unbind();
        swapChain.commandBuffer.unbind();
        swapChain.renderAndPresent();
    }
};

int main() {
    LavaCoreExampleApp application;
    application.run();

    return 0;
}
