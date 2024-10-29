#pragma once

class Application
{
public:
	Application();
  ~Application();
	int Run();
private:
  #ifdef __linux__
  int server_fd;
#endif
};
