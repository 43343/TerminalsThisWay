#include "application.h"
#include "version.h"
#include <iostream>


int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--version") {
        std::cout << "Version: " << currentVersion << " Alpha" << std::endl;
        return 0;
    }

    Application app(argc,argv);

    return app.Run();
}