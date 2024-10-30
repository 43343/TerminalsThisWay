#include "../application.h"
#include <csignal>
#include <unistd.h>
#include "../version.h"
#include <stdexcept>

void signalHandler(int signum) {
    if (signum == SIGUSR1) {
        //loadFromConfig();
    }
}
Application::Application()
{
    signal(SIGUSR1, signalHandler);
}

Application::~Application()
{
}
int Application::Run()
{
    while (true) {
        pause();
    }
  return 0;
}
