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

struct PCModel {
    glm::mat4 model;
    glm::mat4 normalMatrix;
};

struct PCDeferredVP {
    glm::mat4 invViewProj;
    glm::vec3 viewPos;
};

struct GBufferRenderPass {
    lv::Subpass subpass;
    lv::RenderPass renderPass;
    lv::Framebuffer framebuffer;
    lv::CommandBuffer commandBuffer;

    lv::Image diffuseImage;
    lv::Image normalImage;
    lv::Image depthImage;
};

class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool threadPool;
    lv::Instance instance;
    lv::Device device;
    lv::SwapChain swapChain;

    GBufferRenderPass gbufferRenderPass;

    lv::PipelineLayout gbufferPipelineLayout;
    lv::PipelineLayout deferredPipelineLayout;

    lv::ShaderModule vertGBufferShaderModule;
    lv::ShaderModule fragGBufferShaderModule;
    lv::GraphicsPipeline gbufferGraphicsPipeline;

    lv::ShaderModule vertTriangleShaderModule;
    lv::ShaderModule fragDeferredShaderModule;
    lv::GraphicsPipeline deferredGraphicsPipeline;

    lv::VertexDescriptor mainVertexDescriptor = lv::VertexDescriptor(sizeof(MainVertex));
    
    lv::Buffer uniformBuffer;

    lv::DescriptorSet gbufferDescriptorSet;
    lv::DescriptorSet deferredDescriptorSet;

    FirstPersonCamera camera;

    Model sponzaModel;

    float prevTime = 0.0f;

    LavaCoreExampleApp() : Application("deferred_rendering") {
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
        swapChain.init(swapChainCreateInfo);

        uint16_t framebufferWidth, framebufferHeight;
        lvndWindowGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        //Vertex descriptor
        mainVertexDescriptor.addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));
        mainVertexDescriptor.addBinding(1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord));
        mainVertexDescriptor.addBinding(2, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, normal));

        //Render pass
        gbufferRenderPass.diffuseImage.usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        gbufferRenderPass.diffuseImage.format = LV_FORMAT_R8G8B8A8_UNORM;
        gbufferRenderPass.diffuseImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
        gbufferRenderPass.diffuseImage.init(framebufferWidth, framebufferHeight);

        gbufferRenderPass.normalImage.usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        gbufferRenderPass.normalImage.format = LV_FORMAT_R16G16B16A16_SNORM;
        gbufferRenderPass.normalImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
        gbufferRenderPass.normalImage.init(framebufferWidth, framebufferHeight);

        gbufferRenderPass.depthImage.usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        gbufferRenderPass.depthImage.format = swapChain.depthFormat;
        gbufferRenderPass.depthImage.aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT;
        gbufferRenderPass.depthImage.init(framebufferWidth, framebufferHeight);

        gbufferRenderPass.subpass.addColorAttachment({
            .index = 0,
            .layout = LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        });

        gbufferRenderPass.subpass.addColorAttachment({
            .index = 1,
            .layout = LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        });

        gbufferRenderPass.subpass.setDepthAttachment({
            .index = 2,
            .layout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        });

        gbufferRenderPass.renderPass.addSubpass(&gbufferRenderPass.subpass);

        gbufferRenderPass.renderPass.addAttachment({
            .format = gbufferRenderPass.diffuseImage.format,
            .index = 0,
            .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
            .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });

        gbufferRenderPass.renderPass.addAttachment({
            .format = gbufferRenderPass.normalImage.format,
            .index = 1,
            .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
            .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });

        gbufferRenderPass.renderPass.addAttachment({
            .format = gbufferRenderPass.depthImage.format,
            .index = 2,
            .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
            .initialLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            .finalLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
        });

        gbufferRenderPass.renderPass.init();

        gbufferRenderPass.framebuffer.addColorAttachment({
            .image = &gbufferRenderPass.diffuseImage,
            .index = 0
        });

        gbufferRenderPass.framebuffer.addColorAttachment({
            .image = &gbufferRenderPass.normalImage,
            .index = 1
        });

        gbufferRenderPass.framebuffer.setDepthAttachment({
            .image = &gbufferRenderPass.depthImage,
            .index = 2
        });

        gbufferRenderPass.framebuffer.init(&gbufferRenderPass.renderPass);
        gbufferRenderPass.commandBuffer.init();

        //Pipeline layout

        //GBuffer
        gbufferPipelineLayout.pushConstantRanges.resize(1);
        gbufferPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        gbufferPipelineLayout.pushConstantRanges[0].offset = 0;
        gbufferPipelineLayout.pushConstantRanges[0].size = sizeof(PCModel);

        gbufferPipelineLayout.descriptorSetLayouts.resize(2);
        gbufferPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT);

        gbufferPipelineLayout.descriptorSetLayouts[1].addBinding(0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT);

        gbufferPipelineLayout.init();

        //Deferred
        deferredPipelineLayout.pushConstantRanges.resize(1);
        deferredPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT;
        deferredPipelineLayout.pushConstantRanges[0].offset = 0;
        deferredPipelineLayout.pushConstantRanges[0].size = sizeof(PCDeferredVP);

        deferredPipelineLayout.descriptorSetLayouts.resize(1);
        deferredPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT);
        deferredPipelineLayout.descriptorSetLayouts[0].addBinding(1, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT);
        deferredPipelineLayout.descriptorSetLayouts[0].addBinding(2, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT);

        deferredPipelineLayout.init();

        //Graphics pipeline

        //GBuffer
        lv::ShaderModuleCreateInfo vertGBufferShaderModuleCI;
        vertGBufferShaderModuleCI.shaderStage = LV_SHADER_STAGE_VERTEX_BIT;
        vertGBufferShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/vertex/gbuffer.json").c_str());
        vertGBufferShaderModule.init(vertGBufferShaderModuleCI);

        lv::ShaderModuleCreateInfo fragGBufferShaderModuleCI;
        fragGBufferShaderModuleCI.shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT;
        fragGBufferShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/fragment/gbuffer.json").c_str());
        fragGBufferShaderModule.init(fragGBufferShaderModuleCI);

        gbufferGraphicsPipeline.vertexShaderModule = &vertGBufferShaderModule;
        gbufferGraphicsPipeline.fragmentShaderModule = &fragGBufferShaderModule;
        gbufferGraphicsPipeline.pipelineLayout = &gbufferPipelineLayout;
        gbufferGraphicsPipeline.renderPass = &gbufferRenderPass.renderPass;
        gbufferGraphicsPipeline.vertexDescriptor = &mainVertexDescriptor;

        gbufferGraphicsPipeline.config.depthTestEnable = True;
        gbufferGraphicsPipeline.config.cullMode = LV_CULL_MODE_BACK_BIT;

        gbufferGraphicsPipeline.addColorBlendAttachment({0});
        gbufferGraphicsPipeline.addColorBlendAttachment({1});

        gbufferGraphicsPipeline.init();

        //Deferred
        lv::ShaderModuleCreateInfo vertTriangleShaderModuleCI;
        vertTriangleShaderModuleCI.shaderStage = LV_SHADER_STAGE_VERTEX_BIT;
        vertTriangleShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/vertex/triangle.json").c_str());
        vertTriangleShaderModule.init(vertTriangleShaderModuleCI);

        lv::ShaderModuleCreateInfo fragDeferredShaderModuleCI;
        fragDeferredShaderModuleCI.shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT;
        fragDeferredShaderModuleCI.source = lv::readFile((assetDir + "/shaders/compiled/fragment/deferred.json").c_str());
        fragDeferredShaderModule.init(fragDeferredShaderModuleCI);

        deferredGraphicsPipeline.vertexShaderModule = &vertTriangleShaderModule;
        deferredGraphicsPipeline.fragmentShaderModule = &fragDeferredShaderModule;
        deferredGraphicsPipeline.pipelineLayout = &deferredPipelineLayout;
        deferredGraphicsPipeline.renderPass = &swapChain.renderPass;

        deferredGraphicsPipeline.addColorBlendAttachment({0});

        deferredGraphicsPipeline.init();

        //Uniform buffer
        uniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        uniformBuffer.init(sizeof(glm::mat4));

        //Descriptor set
        gbufferDescriptorSet.pipelineLayout = &gbufferPipelineLayout;
        gbufferDescriptorSet.layoutIndex = 0;
        gbufferDescriptorSet.addBinding(uniformBuffer.descriptorInfo(), 0);
        gbufferDescriptorSet.init();

        deferredDescriptorSet.pipelineLayout = &deferredPipelineLayout;
        deferredDescriptorSet.layoutIndex = 0;
        deferredDescriptorSet.addBinding(gbufferRenderPass.diffuseImage.descriptorInfo(), 0);
        deferredDescriptorSet.addBinding(gbufferRenderPass.normalImage.descriptorInfo(), 1);
        deferredDescriptorSet.addBinding(gbufferRenderPass.depthImage.descriptorInfo(), 2);
        deferredDescriptorSet.init();

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.aspectRatio = (float)width / (float)height;

        //Model
        sponzaModel.init(gbufferPipelineLayout, "../assets/models/sponza/scene.gltf", 1, 1);
    }

    ~LavaCoreExampleApp() {
        sponzaModel.destroy();

        gbufferDescriptorSet.destroy();
        deferredDescriptorSet.destroy();

        uniformBuffer.destroy();

        gbufferGraphicsPipeline.destroy();
        vertGBufferShaderModule.destroy();
        fragGBufferShaderModule.destroy();

        deferredGraphicsPipeline.destroy();
        vertTriangleShaderModule.destroy();
        fragDeferredShaderModule.destroy();

        gbufferPipelineLayout.destroy();
        deferredPipelineLayout.destroy();

        swapChain.destroy();
        device.destroy();
        instance.destroy();
    }

    void update() override {
        float crntTime = lvndGetTime();
        float dt = crntTime - prevTime;
        prevTime = crntTime;

        swapChain.acquireNextImage();

        //Rendering

        //GBuffer pass
        gbufferRenderPass.commandBuffer.bind();
        gbufferRenderPass.framebuffer.bind();

        gbufferGraphicsPipeline.bind();

        PCModel model;
        model.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
        model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));

        gbufferGraphicsPipeline.uploadPushConstants(&model, 0);

        camera.inputs(window, dt);
        camera.loadViewProj();

        uniformBuffer.copyDataTo(0, &camera.viewProj);

        gbufferDescriptorSet.bind();

        sponzaModel.render();

        gbufferRenderPass.framebuffer.unbind();
        gbufferRenderPass.commandBuffer.unbind();
        gbufferRenderPass.commandBuffer.submit();

        //Deferrred pass
        swapChain.commandBuffer.bind();
        swapChain.framebuffer.bind();

        deferredGraphicsPipeline.bind();

        PCDeferredVP vp;
        vp.invViewProj = glm::inverse(camera.viewProj);
        vp.viewPos = camera.position;

        deferredGraphicsPipeline.uploadPushConstants(&vp, 0);

        deferredDescriptorSet.bind();

        swapChain.renderFullscreenTriangle();

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
