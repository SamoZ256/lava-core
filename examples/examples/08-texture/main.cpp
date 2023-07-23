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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "application.hpp"

struct MainVertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

MainVertex vertices[4] = {
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}}
};

uint16_t indices[6] = {
    0, 1, 2,
    0, 2, 3
};

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

    lv::Buffer vertexBuffer;
    lv::Buffer indexBuffer;
    lv::Buffer uniformBuffer;

    lv::Image image;
    lv::Sampler sampler;

    lv::DescriptorSet descriptorSet;

    const float nearPlane = 0.01f;
    const float farPlane = 100.0f;
    const float fov = 45.0f;
    float aspectRatio;

    const glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::mat4 projection;

    LavaCoreExampleApp() : Application("08-texture") {
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

        //Vertex descriptor
        mainVertexDescriptor.addBinding(0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position));
        mainVertexDescriptor.addBinding(1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord));

        //Pipeline layout
        mainPipelineLayout.pushConstantRanges.resize(1);
        mainPipelineLayout.pushConstantRanges[0].stageFlags = LV_SHADER_STAGE_VERTEX_BIT;
        mainPipelineLayout.pushConstantRanges[0].offset = 0;
        mainPipelineLayout.pushConstantRanges[0].size = sizeof(glm::mat4);

        mainPipelineLayout.descriptorSetLayouts.resize(1);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT);
        mainPipelineLayout.descriptorSetLayouts[0].addBinding(1, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT);

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
        uniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        uniformBuffer.init(sizeof(glm::mat4));

        //Texture
        image.frameCount = 1;
        image.format = LV_FORMAT_R8G8B8A8_UNORM_SRGB;
        image.viewType = LV_IMAGE_VIEW_TYPE_2D;
        image.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
        image.usage = LV_IMAGE_USAGE_SAMPLED_BIT;
        image.initFromFile("../assets/textures/vulkan_logo.png");

        sampler.filter = LV_FILTER_LINEAR;
        sampler.init();

        //Descriptor set
        descriptorSet.pipelineLayout = &mainPipelineLayout;
        descriptorSet.layoutIndex = 0;
        descriptorSet.addBinding(uniformBuffer.descriptorInfo(), 0);
        descriptorSet.addBinding(sampler.descriptorInfo(image), 1);
        descriptorSet.init();

        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        aspectRatio = (float)width / (float)height;
        projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    ~LavaCoreExampleApp() {
        descriptorSet.destroy();
        
        sampler.destroy();
        image.destroy();

        uniformBuffer.destroy();
        indexBuffer.destroy();
        vertexBuffer.destroy();

        mainGraphicsPipeline.destroy();
        vertMainShaderModule.destroy();
        fragMainShaderModule.destroy();

        mainPipelineLayout.destroy();

        swapChain.destroy();
        device.destroy();
        instance.destroy();
    }

    void update() override {
        swapChain.acquireNextImage();

        swapChain.commandBuffer.bind();

        swapChain.framebuffer.bind();

        //Rendering
        mainGraphicsPipeline.bind();

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)lvndGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        mainGraphicsPipeline.uploadPushConstants(&model, 0);

        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 viewProj = projection * view;

        uniformBuffer.copyDataTo(0, &viewProj);

        descriptorSet.bind();

        vertexBuffer.bindVertexBuffer();
        indexBuffer.bindIndexBuffer(LV_INDEX_TYPE_UINT16);
        indexBuffer.drawIndexed(sizeof(uint16_t));

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
