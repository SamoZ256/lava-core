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

#include <glm/glm.hpp>

#include "application.hpp"

struct MainVertex {
    glm::vec3 position;
    glm::vec3 color;
};

MainVertex vertices[3] = {
    {{ 0.0f,  0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}
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

    LavaCoreExampleApp() : Application("04-vertex_buffer") {
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
        mainVertexDescriptor.addBinding(1, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, color));

        //Pipeline layout
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
    }

    ~LavaCoreExampleApp() {
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

        vertexBuffer.bindVertexBuffer();
        vertexBuffer.draw(sizeof(MainVertex));

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
