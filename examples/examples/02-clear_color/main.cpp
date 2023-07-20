#include "lvcore/threading/thread_pool.hpp"

#include "lvcore/core/instance.hpp"
#include "lvcore/core/device.hpp"
#include "lvcore/core/swap_chain.hpp"

#include "application.hpp"

class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool* threadPool;
    lv::Instance* instance;
    lv::Device* device;
    lv::SwapChain* swapChain;

    LavaCoreExampleApp() : Application("02-clear_color") {
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
    }

    ~LavaCoreExampleApp() {
        delete swapChain;
        delete device;
        delete instance;
    }

    void update() override {
        swapChain->acquireNextImage();

        swapChain->commandBuffer()->beginRecording();
        swapChain->commandBuffer()->beginRenderCommands(swapChain->framebuffer());

        swapChain->commandBuffer()->endRecording();
        swapChain->renderAndPresent();
    }
};

int main() {
    LavaCoreExampleApp application;
    application.run();

    return 0;
}
