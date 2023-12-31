#include "lvcore/threading/thread_pool.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "lvcore/internal/instance.hpp"
#include "lvcore/internal/device.hpp"
#include "lvcore/internal/swap_chain.hpp"
#include "lvcore/internal/pipeline_layout.hpp"
#include "lvcore/internal/shader_module.hpp"
#include "lvcore/internal/graphics_pipeline.hpp"

#include "application.hpp"

class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool* threadPool;
    lv::Instance* instance;
    lv::Device* device;
    lv::SwapChain* swapChain;

    lv::PipelineLayout* mainPipelineLayout;

    lv::ShaderModule* vertMainShaderModule;
    lv::ShaderModule* fragMainShaderModule;
    lv::GraphicsPipeline* mainGraphicsPipeline;

    LavaCoreExampleApp() : Application("03-triangle") {
        threadPool = new lv::ThreadPool({});

        instance = new lv::Instance({
            .applicationName = exampleName.c_str(),
            .validationEnable = True
        });

        device = new lv::Device({
            .window = window,
            .threadPool = threadPool
        });

        swapChain = new lv::SwapChain({
            .window = window,
            .vsyncEnable = True,
            .maxFramesInFlight = 2,
            .clearAttachment = True
        });

        //Pipeline layout
        mainPipelineLayout = new lv::PipelineLayout({});

        //Graphics pipeline
        vertMainShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_VERTEX_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/vertex/main.json").c_str())
        });

        fragMainShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/main.json").c_str())
        });

        mainGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertMainShaderModule,
            .fragmentShaderModule = fragMainShaderModule,
            .pipelineLayout = mainPipelineLayout,
            .renderPass = swapChain->renderPass(),
            .colorBlendAttachments = {
                {0}
            }
        });
    }

    ~LavaCoreExampleApp() {
        delete mainGraphicsPipeline;
        delete vertMainShaderModule;
        delete fragMainShaderModule;

        delete mainPipelineLayout;

        delete swapChain;
        delete device;
        delete instance;
    }

    void update() override {
        swapChain->acquireNextImage();

        swapChain->commandBuffer()->beginRecording();
        swapChain->commandBuffer()->beginRenderCommands(swapChain->framebuffer());

        swapChain->commandBuffer()->cmdBindGraphicsPipeline(mainGraphicsPipeline);

        swapChain->commandBuffer()->cmdDraw(3);

        swapChain->commandBuffer()->endRecording();
        swapChain->renderAndPresent();
    }
};

int main() {
    LavaCoreExampleApp application;
    application.run();

    return 0;
}
