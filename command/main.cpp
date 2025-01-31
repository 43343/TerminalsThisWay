#include <iostream>
#include <windows.h>
#include "application.h"

int main(int argc, char* argv[]) {
    Application app(argv);
    return app.Run();
}