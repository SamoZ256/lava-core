#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#define LVND_DEBUG
#include "lvnd/lvnd.h"

class Application {
public:
    LvndWindow* window;

    std::string exampleName;
    std::string assetDir;

    Application(std::string aExampleName);

    ~Application();

    void run();

    virtual void start() {}

    virtual void update() {}
};

#endif
