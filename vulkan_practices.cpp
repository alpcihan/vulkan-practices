#include "engine/application.h"

int main() {
    eng::Application* app = new eng::Application();

    app->run();

    delete app;

    return 0;
}