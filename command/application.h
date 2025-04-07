#include <windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>

class Application
{
public:
	Application(char* argv[]);
	int Run();
private:
	bool isValidToken(const std::wstring& token);
	std::thread workerThread;
	std::atomic<bool> stopWorkingThread = false;
	std::mutex queueMutex;
	std::queue<std::wstring> commandQueue;
	int startedProcessIDsCMD = 0;
	void commandProcessingLoop();
	void readFromSharedMemory();
	void processNextCommand(std::wstring& command);
	bool IsProcessRunning(DWORD processID);
	bool isTerminalReady();
	std::wstring getAppdataFolder();
	static BOOL WINAPI IgnoreCtrlHandler(DWORD dwCtrlType);
};