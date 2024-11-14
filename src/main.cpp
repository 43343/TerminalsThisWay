#include "application.h"
#include "version.h"
#include <iostream>


int main(int argc, char* argv[]) {

    Application app(argc,argv);

    return app.Run();
}
