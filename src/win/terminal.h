#pragma once
#include <string>
#include <windows.h>    
#include <vector>


class Terminal
{
public:
	Terminal();
	void createProcessCMD(const std::string& path);
	void sendCommandToCMD(const std::string& command);
private:
	HANDLE CMD_READ, PARENT_WRITE;
	DWORD startedProcessIDs = 0;
	void handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo);
	bool IsProcessRunning(DWORD processID);
};
