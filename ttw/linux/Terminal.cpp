#include <exception>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include "Terminal.h"

Terminal::Terminal()
{}

bool Terminal::IsProcessRunning(const pid_t& processId)
{
  std::string procPath = "/proc/" + std::to_string(processId);
  std::ifstream procFile(procPath + "/status");
  return procFile.good();
}
void Terminal::createProcessCmd(const std::string& path)
{
  if((currentPathTerminal != path || !IsProcessRunning(pid)) && pid != 0)
  {
    pid = 0;
  }
  currentPathTerminal = path;
  if(pid == 0)
  {
    pid_t forkResult = fork();
    if(forkResult == 0)
    {
      setsid();
      execlp(path.c_str(), path.c_str(), nullptr);
    }
    else {
      pid = forkResult;
    }
  }
}
void Terminal::sendCommandToCmd(const std::string& command, const bool& createCmd)
{
  if(createCmd)
  {
    createProcessCmd("xterm");
  }
}

