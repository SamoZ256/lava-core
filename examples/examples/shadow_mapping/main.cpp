#include "lvcore/threading/thread_pool.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "lvcore/internal/instance.hpp"
#include "lvcore/internal/device.hpp"
#include "lvcore/internal/swap_chain.hpp"
#include "lvcore/internal/pipeline_layout.hpp"
#include "lvcore/internal/shader_module.hpp"
#include "lvcore/internal/graphics_pipeline.hpp"
#include "lvcore/internal/vertex_descriptor.hpp"
#include "lvcore/internal/buffer.hpp"
#include "lvcore/internal/descriptor_set.hpp"
#include "lvcore/internal/image.hpp"
#include "lvcore/internal/sampler.hpp"
#include "lvcore/internal/render_pass.hpp"
#include "lvcore/internal/framebuffer.hpp"
#include "lvcore/internal/command_buffer.hpp"

#include "model.hpp"
#include "first_person_camera.hpp"

#include "application.hpp"

#define SHADOW_MAP_SIZE 4096

struct PCModel {
    glm::mat4 model;
    glm::mat4 normalMatrix;
};

struct UBOVP {
    glm::mat4 viewProj;
    glm::mat4 shadowViewProj;
    glm::vec3 viewPos;
};

struct ShadowRenderPass {
    lv::Subpass subpass;
    lv::RenderPass renderPass;
    lv::Framebuffer framebuffer;
    lv::CommandBuffer commandBuffer;

    lv::Image depthImage;
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
    
    lv::Buffer shadowUniformBuffer;
    lv::Buffer mainUniformBuffer;

    lv::DescriptorSet shadowDescriptorSet;
    lv::DescriptorSet mainDescriptorSet;

    FirstPersonCamera camera;

    Model sponzaModel;

    float prevTime = 0.0f;

    LavaCoreExampleApp() : Application("shadow_mapping") {
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
        shadowRenderPass.depthImage.aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT;
        shadowRenderPass.depthImage.init(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        shadowRenderPass.depthSampler.filter = LV_FILTER_LINEAR;
        shadowRenderPass.depthSampler.compareEnable = True;
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

        shadowRenderPass.framebuffer.setDepthAttachment({
            .index = 0,
            .image = &shadowRenderPass.depthImage
        });

        shadowRenderPass.framebuffer.init(&shadowRenderPass.renderPass);
        shadowRenderPass.commandBuffer.init();

        //Pipeline layout

        //Shadow
        shadowPipelineLayout.pushConstantRanges.resize(1);
        shadowPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        shadowPipelineLayout.pushConstantRanges[0].offset = 0;
        shadowPipelineLayout.pushConstantRanges[0].size = sizeof(glm::mat4);

        shadowPipelineLayout.descriptorSetLayouts.resize(1);
        shadowPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT);

        shadowPipelineLayout.init();

        //Main
        mainPipelineLayout.pushConstantRanges.resize(1);
        mainPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        mainPipelineLayout.pushConstantRanges[0].offset = 0;
        mainPipelineLayout.pushConstantRanges[0].size = sizeof(PCModel);

        mainPipelineLayout.descriptorSetLayouts.resize(2);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT | LV_SHADER_STAGE_FRAGMENT_BIT);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(1, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT);

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
        shadowUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        shadowUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        shadowUniformBuffer.init(sizeof(glm::mat4));

        mainUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mainUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        mainUniformBuffer.init(sizeof(UBOVP));

        //Descriptor set
        shadowDescriptorSet.pipelineLayout = &shadowPipelineLayout;
        shadowDescriptorSet.layoutIndex = 0;
        shadowDescriptorSet.addBinding(shadowUniformBuffer.descriptorInfo(), 0);
        shadowDescriptorSet.init();

        mainDescriptorSet.pipelineLayout = &mainPipelineLayout;
        mainDescriptorSet.layoutIndex = 0;
        mainDescriptorSet.addBinding(mainUniformBuffer.descriptorInfo(), 0);
        mainDescriptorSet.addBinding(shadowRenderPass.depthSampler.descriptorInfo(shadowRenderPass.depthImage), 1);
        mainDescriptorSet.init();

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.aspectRatio = (float)width / (float)height;

        //Model
        sponzaModel.init(mainPipelineLayout, "../assets/models/sponza/scene.gltf", 1, 1);
    }

    ~LavaCoreExampleApp() {
        sponzaModel.destroy();

        shadowDescriptorSet.destroy();
        mainDescriptorSet.destroy();

        shadowUniformBuffer.destroy();
        mainUniformBuffer.destroy();

        shadowGraphicsPipeline.destroy();
        vertShadowShaderModule.destroy();
        fragShadowShaderModule.destroy();

        mainGraphicsPipeline.destroy();
        vertMainShaderModule.destroy();
        fragMainShaderModule.destroy();

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

        //Shadow pass
        shadowRenderPass.commandBuffer.bind();
        shadowRenderPass.framebuffer.bind();

        shadowGraphicsPipeline.bind();

        PCModel model;
        model.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
        model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));

        shadowGraphicsPipeline.uploadPushConstants(&model.model, 0);
        
        glm::mat4 shadowProjection = glm::ortho(-18.0f, 18.0f, -18.0f, 18.0f, -25.0f, 25.0f);
        glm::mat4 shadowView = glm::lookAt(glm::normalize(glm::vec3(-2.0, 4.0, -1.0)), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 shadowViewProj = shadowProjection * shadowView;

        shadowUniformBuffer.copyDataTo(0, &shadowViewProj);

        shadowDescriptorSet.bind();

        sponzaModel.renderNoTextures();

        shadowRenderPass.framebuffer.unbind();
        shadowRenderPass.commandBuffer.unbind();
        shadowRenderPass.commandBuffer.submit();

        //Main pass
        swapChain.commandBuffer.bind();
        swapChain.framebuffer.bind();

        mainGraphicsPipeline.bind();

        mainGraphicsPipeline.uploadPushConstants(&model, 0);

        UBOVP vp;
        vp.viewProj = camera.viewProj;
        vp.shadowViewProj = shadowViewProj;
        vp.viewPos = camera.position;

        mainUniformBuffer.copyDataTo(0, &vp);

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
