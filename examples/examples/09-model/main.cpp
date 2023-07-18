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
#include "lvcore/core/image.hpp"
#include "lvcore/core/sampler.hpp"

#include "model.hpp"
#include "first_person_camera.hpp"

#include "application.hpp"

class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool threadPool;
    lv::Instance instance;
    lv::Device device;
    lv::SwapChain swapChain;

    lv::PipelineLayout mainPipelineLayout;

    lv::ShaderModule vertMainShaderModule;
    lv::ShaderModule fragMainShaderModule;
    lv::GraphicsPipeline mainGraphicsPipeline;

    lv::VertexDescriptor mainVertexDescriptor = lv::VertexDescriptor(sizeof(MainVertex));
    
    lv::Buffer uniformBuffer;

    lv::DescriptorSet descriptorSet;

    FirstPersonCamera camera;

    Model sponzaModel;

    float prevTime = 0.0f;

    LavaCoreExampleApp() : Application("09-model") {
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
        swapChainCreateInfo.createDepthAttachment = true;
        swapChain.init(swapChainCreateInfo);

        //Vertex descriptor
        mainVertexDescriptor.addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));
        mainVertexDescriptor.addBinding(1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord));

        //Pipeline layout
        mainPipelineLayout.pushConstantRanges.resize(1);
        mainPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        mainPipelineLayout.pushConstantRanges[0].offset = 0;
        mainPipelineLayout.pushConstantRanges[0].size = sizeof(glm::mat4);

        mainPipelineLayout.descriptorSetLayouts.resize(2);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT);

        mainPipelineLayout.descriptorSetLayouts[1].addBinding(0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT);

        mainPipelineLayout.init();

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

        mainGraphicsPipeline.config.depthTestEnable = true;
        mainGraphicsPipeline.config.cullMode = LV_CULL_MODE_BACK_BIT;

        mainGraphicsPipeline.addColorBlendAttachment({0});

        mainGraphicsPipeline.init();

        //Uniform buffer
        uniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        uniformBuffer.init(sizeof(glm::mat4));

        //Descriptor set
        descriptorSet.pipelineLayout = &mainPipelineLayout;
        descriptorSet.layoutIndex = 0;
        descriptorSet.addBinding(uniformBuffer.descriptorInfo(), 0);
        descriptorSet.init();

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.aspectRatio = (float)width / (float)height;

        //Model
        sponzaModel.init(mainPipelineLayout, "../assets/models/sponza/scene.gltf", 1, 1);
    }

    ~LavaCoreExampleApp() {
        sponzaModel.destroy();

        descriptorSet.destroy();

        uniformBuffer.destroy();

        mainGraphicsPipeline.destroy();
        vertMainShaderModule.destroy();
        fragMainShaderModule.destroy();

        mainPipelineLayout.destroy();

        swapChain.destroy();
        device.destroy();
        instance.destroy();

        Application::~Application();
    }

    void update() override {
        float crntTime = lvndGetTime();
        float dt = crntTime - prevTime;
        prevTime = crntTime;

        swapChain.acquireNextImage();

        swapChain.commandBuffer.bind();

        swapChain.framebuffer.bind();

        //Rendering
        mainGraphicsPipeline.bind();

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

        mainGraphicsPipeline.uploadPushConstants(&model, 0);

        camera.inputs(window, dt);
        camera.loadViewProj();

        uniformBuffer.copyDataTo(0, &camera.viewProj);

        descriptorSet.bind();

        sponzaModel.render();

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
