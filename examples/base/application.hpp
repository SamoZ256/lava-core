#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#define LVND_DEBUG
#include "lvnd/lvnd.h"

#include "lvcore/lvcore.hpp"

class Application {
public:
    LvndWindow* window;

    lv::Instance* instance;

    std::string exampleName;
    std::string assetDir;

    Application(std::string aExampleName, int argc, char* argv[]);

    ~Application();

    void run();

    virtual void start() {}

    virtual void update() {}
};

#endif
