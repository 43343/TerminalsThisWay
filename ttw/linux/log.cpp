#include "log.h"

std::string logFile()
{
  const char *homeDir = std::getenv("HOME");
  if(homeDir)
  {
    std::string m_logFile = std::string(homeDir) + "/.config/ttw/log.txt";
    std::filesystem::create_directories(std::filesystem::path(m_logFile).parent_path());
    return m_logFile;
  }
  return "";
}

void clear_log()
{
  std::string m_logFile = logFile();
  std::ofstream ofs(m_logFile, std::ios::trunc);
  ofs.is_open();
  ofs.close();
}

void write_log(const std::string& str)
{
  std::string m_logFile = logFile();
  std::ofstream ofs(m_logFile, std::ios::app);
  ofs.is_open();
  ofs << str << "\n";
  ofs.close();

}
