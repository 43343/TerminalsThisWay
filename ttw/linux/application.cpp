#include "../application.h"
#include <csignal>
#include <unistd.h>
#include "../version.h"
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include "x11/main_xlib.h"
#include <locale.h>

void signalHandler(int signum) {
    if (signum == SIGUSR1) {
        //loadFromConfig();
        std::cout << "loadFromConfig()";
    }
}
Application::Application(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
    //signal(SIGUSR1, signalHandler);
    if(argc > 1 && std::string(argv[1]) == "--version")
    {
      std::cout << "Version: " << currentVersion << " Alphas" << std::endl;
      exit(0);
    }
    std::cout << "Initialize applaiction";
}

Application::~Application()
{
}
int Application::Run()
{
  printf("loh");
  run_xlib();
  return 0;
}
