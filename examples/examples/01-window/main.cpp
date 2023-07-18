#include "application.hpp"

class LavaCoreExampleApp : public Application {
public:
    LavaCoreExampleApp() : Application("01-window") {

    }
};

int main() {
    LavaCoreExampleApp application;
    application.run();

    return 0;
}
