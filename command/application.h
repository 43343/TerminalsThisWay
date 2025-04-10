#include <windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>

class Application
{
public:
	Application(char* argv[]);
	int Run();
private:
	struct CommandParam
	{
		int startedProcessIDsCMD;
		std::wstring command;
	};
	bool isValidToken(const std::wstring& token);
	std::thread workerThread;
	std::atomic<bool> stopWorkingThread = false;
	std::mutex queueMutex;
	std::queue<CommandParam> commandQueue;
	void commandProcessingLoop();
	void readFromSharedMemory();
	void processNextCommand(CommandParam& commandParam);
	bool IsProcessRunning(DWORD processID);
	bool isTerminalReady();
	std::wstring getAppdataFolder();
	static BOOL WINAPI IgnoreCtrlHandler(DWORD dwCtrlType);
};