#pragma once
#include <string>
#include <windows.h>    
#include <vector>


class Terminal
{
public:
	Terminal();
	void sendCommandToCMD(const std::wstring& command, const bool& createCmd);
private:
	DWORD startedProcessIDs = 0;
	void handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo);
	bool IsProcessRunning(DWORD processID);
	void createProcessCMD(const std::wstring& path);
	std::wstring currentPathTerminal;
};
