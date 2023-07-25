#include "application.hpp"

Application* app = nullptr;

Application::Application(std::string aExampleName, int argc, char* argv[]) : exampleName(aExampleName), assetDir("../examples/examples/" + exampleName) {
    lvndInit();

    window = lvndCreateWindow(960, 540, exampleName.c_str());

    lv::RenderAPI renderAPI = lv::defaultRenderAPI;
    if (argc > 1) {
        for (uint32_t i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--vulkan") == 0)
                renderAPI = lv::RenderAPI::Vulkan;
            else if (strcmp(argv[i], "--metal") == 0)
                renderAPI = lv::RenderAPI::Metal;
        }
    }

    instance = new lv::Instance({
        .renderAPI = renderAPI,
        .applicationName = exampleName.c_str(),
        .validationEnable = lv::True
    });

    app = this;
}

Application::~Application() {
    lvndDestroyWindow(window);
}

void Application::run() {
    lvndMainLoop(window, [](){ app->start(); }, [](){ app->update(); });
}
