#include "../application.h"
#include <csignal>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include "../version.h"
#include <stdexcept>
const char* SOCKET_PATH = "/tmp/ttw_socket";
bool running = true;
void signalHandler(int signum)
{
  running = false;
  unlink(SOCKET_PATH);
}
Application::Application()
{
  signal(SIGTERM, signalHandler);

  server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(server_fd == -1) {
    throw std::runtime_error("Socket creation failed");
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
  unlink(SOCKET_PATH);
  if(bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == -1)
  {
    throw std::runtime_error("Bind failed");
  }
  if(listen(server_fd, 5) == -1)
  {
    throw std::runtime_error("Listen failed");
  }
}

Application::~Application()
{
  close(server_fd);
  unlink(SOCKET_PATH);
}
int Application::Run()
{
  while(running)
  {
    int client_fd = accept(server_fd, nullptr, nullptr);
    if(client_fd == -1)
    {
      continue;
    }

    write(client_fd, currentVersion.c_str(), currentVersion.size());
    close(client_fd);

  }
  return 0;
}
