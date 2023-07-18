#include "application.hpp"

Application* app = nullptr;

Application::Application(std::string aExampleName) : exampleName(aExampleName), assetDir("../examples/examples/" + exampleName) {
    lvndInit();

    window = lvndCreateWindow(960, 540, exampleName.c_str());

    app = this;
}

Application::~Application() {
    lvndDestroyWindow(window);
}

void Application::run() {
    lvndMainLoop(window, [](){ app->start(); }, [](){ app->update(); });
}
