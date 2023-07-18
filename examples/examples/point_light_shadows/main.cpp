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
#include "lvcore/core/render_pass.hpp"
#include "lvcore/core/framebuffer.hpp"
#include "lvcore/core/command_buffer.hpp"

#include "model.hpp"
#include "first_person_camera.hpp"

#include "application.hpp"

#define SHADOW_MAP_SIZE 1024
#define SHADOW_FAR_PLANE 10.0f

struct PCModel {
    glm::mat4 model;
    glm::mat4 normalMatrix;
};

struct UBOShadowVP {
    glm::mat4 viewProj;
    glm::vec3 lightPos;
};

struct UBOVP {
    glm::mat4 viewProj;
    glm::mat4 shadowProj;
    glm::vec3 viewPos;
};

struct UBOLight {
    glm::vec4 position; //HACK: glm alignment not working
    glm::vec4 color;

    float constant;
    float linear;
    float quadratic;
};

struct ShadowRenderPass {
    lv::Subpass subpass;
    lv::RenderPass renderPass;
    lv::Framebuffer framebuffers[6];
    lv::CommandBuffer commandBuffer;

    lv::Image depthImage;
    lv::Image depthImageViews[6];
    lv::Sampler depthSampler;
};

class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool threadPool;
    lv::Instance instance;
    lv::Device device;
    lv::SwapChain swapChain;

    lv::VertexDescriptor shadowVertexDescriptor = lv::VertexDescriptor(sizeof(MainVertex));
    lv::VertexDescriptor mainVertexDescriptor = lv::VertexDescriptor(sizeof(MainVertex));

    ShadowRenderPass shadowRenderPass;

    lv::PipelineLayout shadowPipelineLayout;
    lv::PipelineLayout mainPipelineLayout;

    lv::ShaderModule vertShadowShaderModule;
    lv::ShaderModule fragShadowShaderModule;
    lv::GraphicsPipeline shadowGraphicsPipeline;

    lv::ShaderModule vertMainShaderModule;
    lv::ShaderModule fragMainShaderModule;
    lv::GraphicsPipeline mainGraphicsPipeline;
    
    lv::Buffer shadowUniformBuffers[6]; //TODO: use dynamic offsets instead
    lv::Buffer mainUniformBuffer;
    lv::Buffer lightUniformBuffer;

    lv::DescriptorSet shadowDescriptorSets[6];
    lv::DescriptorSet mainDescriptorSet;

    FirstPersonCamera camera;

    Model sponzaModel;

    UBOLight light;
    glm::vec3 lightDirections[6] = {
        { 1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f, -1.0f}
    };
    glm::vec3 upDirections[6] = {
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f}
    };

    float prevTime = 0.0f;

    LavaCoreExampleApp() : Application("point_light_shadows") {
        lv::ThreadPoolCreateInfo threadPoolCreateInfo;
        threadPool.init(threadPoolCreateInfo);

        lv::InstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.applicationName = "Lava Core example";
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
        shadowVertexDescriptor.addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));

        mainVertexDescriptor.addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));
        mainVertexDescriptor.addBinding(1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord));
        mainVertexDescriptor.addBinding(2, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, normal));

        //Render pass
        shadowRenderPass.depthImage.usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        shadowRenderPass.depthImage.format = swapChain.depthFormat;
        shadowRenderPass.depthImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
        shadowRenderPass.depthImage.aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT;
        shadowRenderPass.depthImage.init(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        for (uint8_t i = 0; i < 6; i++) {
            shadowRenderPass.depthImage.newImageView(shadowRenderPass.depthImageViews[i], LV_IMAGE_VIEW_TYPE_2D, i, 1, 0, 1);
        }
        shadowRenderPass.depthSampler.filter = LV_FILTER_LINEAR;
        shadowRenderPass.depthSampler.compareEnable = LV_TRUE;
        shadowRenderPass.depthSampler.compareOp = LV_COMPARE_OP_LESS;
        shadowRenderPass.depthSampler.init();

        shadowRenderPass.subpass.setDepthAttachment({
            .index = 0,
            .layout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        });

        shadowRenderPass.renderPass.addSubpass(&shadowRenderPass.subpass);

        shadowRenderPass.renderPass.addAttachment({
            .index = 0,
            .format = shadowRenderPass.depthImage.format,
            .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
            //TODO: configure image layouts
        });

        shadowRenderPass.renderPass.init();

        for (uint8_t i = 0; i < 6; i++) {
            shadowRenderPass.framebuffers[i].setDepthAttachment({
                .index = 0,
                .image = &shadowRenderPass.depthImageViews[i]
            });

            shadowRenderPass.framebuffers[i].init(&shadowRenderPass.renderPass);
        }
        shadowRenderPass.commandBuffer.init();

        //Pipeline layout

        //Shadow
        shadowPipelineLayout.pushConstantRanges.resize(1);
        shadowPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        shadowPipelineLayout.pushConstantRanges[0].offset = 0;
        shadowPipelineLayout.pushConstantRanges[0].size = sizeof(glm::mat4);

        shadowPipelineLayout.descriptorSetLayouts.resize(1);
        shadowPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT | LV_SHADER_STAGE_FRAGMENT_BIT);

        shadowPipelineLayout.init();

        //Main
        mainPipelineLayout.pushConstantRanges.resize(1);
        mainPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        mainPipelineLayout.pushConstantRanges[0].offset = 0;
        mainPipelineLayout.pushConstantRanges[0].size = sizeof(PCModel);

        mainPipelineLayout.descriptorSetLayouts.resize(2);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT | LV_SHADER_STAGE_FRAGMENT_BIT);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(1, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_FRAGMENT_BIT);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(2, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT);

        mainPipelineLayout.descriptorSetLayouts[1].addBinding(0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT);

        mainPipelineLayout.init();

        //Graphics pipeline

        //Shadow
        lv::ShaderModuleCreateInfo vertShadowShaderModuleCI;
        vertShadowShaderModuleCI.shaderStage = LV_SHADER_STAGE_VERTEX_BIT;
        vertShadowShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/vertex/shadow.json").c_str());
        vertShadowShaderModule.init(vertShadowShaderModuleCI);

        lv::ShaderModuleCreateInfo fragShadowShaderModuleCI;
        fragShadowShaderModuleCI.shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT;
        fragShadowShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/fragment/shadow.json").c_str());
        fragShadowShaderModule.init(fragShadowShaderModuleCI);

        shadowGraphicsPipeline.vertexShaderModule = &vertShadowShaderModule;
        shadowGraphicsPipeline.fragmentShaderModule = &fragShadowShaderModule;
        shadowGraphicsPipeline.pipelineLayout = &shadowPipelineLayout;
        shadowGraphicsPipeline.renderPass = &shadowRenderPass.renderPass;
        shadowGraphicsPipeline.vertexDescriptor = &shadowVertexDescriptor;

        shadowGraphicsPipeline.config.depthTestEnable = true;
        shadowGraphicsPipeline.config.cullMode = LV_CULL_MODE_BACK_BIT;

        shadowGraphicsPipeline.init();

        //Main
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
        for (uint8_t i = 0; i < 6; i++) {
            shadowUniformBuffers[i].usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            shadowUniformBuffers[i].memoryType = LV_MEMORY_TYPE_SHARED;
            shadowUniformBuffers[i].init(sizeof(UBOShadowVP));
        }

        mainUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mainUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        mainUniformBuffer.init(sizeof(UBOVP));

        lightUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        lightUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        lightUniformBuffer.init(sizeof(UBOLight));

        //Descriptor set
        for (uint8_t i = 0; i < 6; i++) {
            shadowDescriptorSets[i].pipelineLayout = &shadowPipelineLayout;
            shadowDescriptorSets[i].layoutIndex = 0;
            shadowDescriptorSets[i].addBinding(shadowUniformBuffers[i].descriptorInfo(), 0);
            shadowDescriptorSets[i].init();
        }

        mainDescriptorSet.pipelineLayout = &mainPipelineLayout;
        mainDescriptorSet.layoutIndex = 0;
        mainDescriptorSet.addBinding(mainUniformBuffer.descriptorInfo(), 0);
        mainDescriptorSet.addBinding(lightUniformBuffer.descriptorInfo(), 1);
        mainDescriptorSet.addBinding(shadowRenderPass.depthSampler.descriptorInfo(shadowRenderPass.depthImage), 2);
        mainDescriptorSet.init();

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.aspectRatio = (float)width / (float)height;

        //Model
        sponzaModel.init(mainPipelineLayout, "../assets/models/sponza/scene.gltf", 1, 1);

        //Light
        light.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        light.constant = 0.5f;
        light.linear = 0.045f;
        light.quadratic = 0.016f;
    }

    ~LavaCoreExampleApp() {
        sponzaModel.destroy();

        for (uint8_t i = 0; i < 6; i++)
            shadowDescriptorSets[i].destroy();
        mainDescriptorSet.destroy();

        for (uint8_t i = 0; i < 6; i++)
            shadowUniformBuffers[i].destroy();
        mainUniformBuffer.destroy();
        lightUniformBuffer.destroy();

        mainGraphicsPipeline.destroy();
        vertMainShaderModule.destroy();
        fragMainShaderModule.destroy();

        shadowGraphicsPipeline.destroy();
        vertShadowShaderModule.destroy();
        fragShadowShaderModule.destroy();

        shadowPipelineLayout.destroy();
        mainPipelineLayout.destroy();

        //TODO: destroy render pass

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

        camera.inputs(window, dt);
        camera.loadViewProj();

        //Rendering

        PCModel model;
        model.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
        model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));

        glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, SHADOW_FAR_PLANE);

        if (lvndGetKeyState(window, LVND_KEY_SPACE) != LVND_STATE_PRESSED)
            light.position = glm::vec4(sin(lvndGetTime() * 0.1f) * 5.0f, 2.0f, cos(lvndGetTime() * 0.1f) * 5.0f, 0.0f);

        //Shadow pass
        shadowRenderPass.commandBuffer.bind();
        for (uint8_t i = 0; i < 6; i++) {
            shadowRenderPass.framebuffers[i].bind();

            shadowGraphicsPipeline.bind();

            shadowGraphicsPipeline.uploadPushConstants(&model.model, 0);
            
            UBOShadowVP shadowVP;
            glm::mat4 shadowView = glm::lookAt(glm::vec3(light.position), glm::vec3(light.position) + lightDirections[i], upDirections[i]);
            shadowVP.viewProj = shadowProjection * shadowView;
            shadowVP.lightPos = light.position;

            shadowUniformBuffers[i].copyDataTo(0, &shadowVP);

            shadowDescriptorSets[i].bind();

            sponzaModel.renderNoTextures();

            shadowRenderPass.framebuffers[i].unbind();
        }
        shadowRenderPass.commandBuffer.unbind();
        shadowRenderPass.commandBuffer.submit();

        //Main pass
        swapChain.commandBuffer.bind();
        swapChain.framebuffer.bind();

        mainGraphicsPipeline.bind();

        mainGraphicsPipeline.uploadPushConstants(&model, 0);

        UBOVP vp;
        vp.viewProj = camera.viewProj;
        vp.shadowProj = shadowProjection;
        vp.viewPos = camera.position;

        mainUniformBuffer.copyDataTo(0, &vp);

        lightUniformBuffer.copyDataTo(0, &light);

        mainDescriptorSet.bind();

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
