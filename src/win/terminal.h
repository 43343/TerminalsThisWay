#pragma once
#include <string>
#include <windows.h>    
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

class Terminal
{
public:
	Terminal();
	void sendCommandToCMD(const std::wstring& command, const bool& createCmd);
private:
	std::queue<std::wstring> commandQueue;
	std::condition_variable cv;
	std::atomic<bool> isBusy = false;
	std::atomic<bool> stopProcessing = false;
	std::thread workerThread;
	std::mutex queueMutex;
	void commandProcessingLoop();
	void processNextCommand(const std::wstring& command);
	bool isTerminalReady();
	DWORD startedProcessIDs = 0;
	void handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo);
	bool IsProcessRunning(DWORD processID);
	void createProcessCMD(const std::wstring& path);
	std::wstring currentPathTerminal;
};
