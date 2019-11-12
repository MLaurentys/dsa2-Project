// Include standard headers
#include "main.h"
using namespace Simplex;

int main(void) {
    Application* app = new Application("Doodle Jump", 400, 400);
    app->Run();
    delete app;
    return 0;
}