#include <string>
#include <sys/types.h>

class Terminal
{
  public:
    Terminal();
    void sendCommandToCmd(const std::string& command, const bool& createCmd);
  private:
    bool IsProcessRunning(const pid_t& processId);
    void createProcessCmd(const std::string& path);
    pid_t pid = 0;
    std::string currentPathTerminal;
};
