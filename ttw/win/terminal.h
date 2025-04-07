#pragma once
#include <string>
#include <vector>
#include <windows.h>

class Terminal
{
public:
	Terminal();
	void sendCommandToCMD(const std::wstring& command, const bool& createCmd);
	void bringProcessWindowToTop();
private:
	DWORD startedProcessIDsCMD = 0;
	DWORD startedProcessIDsCommand = 0;
	bool IsProcessRunning(DWORD processID);
	void createProcessCMD(const std::wstring& path);
	void createProcessCommand();
	std::wstring currentPathTerminal;
	std::wstring generateToken();
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
};
