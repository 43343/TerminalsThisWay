#pragma once
#include <string>
#include <windows.h>    
#include <vector>


class Terminal
{
public:
	Terminal();
	void createProcessCMD(const std::wstring& path);
	void sendCommandToCMD(const std::wstring& command);
private:
	HANDLE CMD_READ, PARENT_WRITE;
	DWORD startedProcessIDs = 0;
	void handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo);
	bool IsProcessRunning(DWORD processID);
	std::wstring currentPathTerminal;
	std::string utf16ToUtf8(const std::wstring& utf16Str);
};
