#define _WIN32_WINNT 0x0A00
#include "terminal.h"
#include <windows.h>
#include <consoleapi2.h>
#include <tlhelp32.h>
#include <codecvt>
#include <iostream>
#include "Config/configManager.h"

Terminal::Terminal() {}

bool Terminal::IsProcessRunning(DWORD processID) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshot, &entry)) {
		do {
			if (entry.th32ProcessID == processID) {
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return false;
}

void Terminal::createProcessCMD(const std::wstring& path)
{
	std::wstring pathToTerminalQuotes = path;
	size_t pos = 0;
	while ((pos = pathToTerminalQuotes.find(L'"', pos)) != std::wstring::npos) {
		pathToTerminalQuotes.erase(pos, 1);
	}
	if (currentPathTerminal != pathToTerminalQuotes && startedProcessIDs != 0)
	{
		sendCommandToCMD(L"exit", false);
		startedProcessIDs = 0;
	}
	currentPathTerminal = pathToTerminalQuotes;
	if (IsProcessRunning(startedProcessIDs) && startedProcessIDs != 0) {
		std::cout << "A terminal process is already running." << std::endl;
		return;
	}
	FreeConsole();

	STARTUPINFOW PSTARTUPINFO = { 0 };
	PROCESS_INFORMATION PPROCESSINFO = { 0 };
	SECURITY_ATTRIBUTES SECURITYATTR;
	BOOL success = CreateProcessW(pathToTerminalQuotes.c_str(),
		NULL,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&PSTARTUPINFO,
		&PPROCESSINFO);

	startedProcessIDs = PPROCESSINFO.dwProcessId;

	Sleep(2000);
	sendCommandToCMD(L"chcp 65001", false);
}

void Terminal::handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo)
{
	if (!CloseHandle(processinfo.hProcess)) printf("[-] Could not close process handle\n");
	if (!CloseHandle(processinfo.hThread)) printf("[-] Could not close thread handle\n");
}
void Terminal::sendCommandToCMD(const std::wstring& command, const bool& createCmd)
{
	if (createCmd)
	{
		createProcessCMD(ConfigManager::getInstance().getConfig().pathToTerminal);
	}
	if (IsProcessRunning(startedProcessIDs) && startedProcessIDs != 0)
	{
		FreeConsole();
		if (!AttachConsole(startedProcessIDs))
		{
			std::cerr << "ErrorAttached\r\n";
			MessageBoxA(NULL, "Couldn't connect to the terminal.", "Error", MB_ICONERROR | MB_OK);
#ifdef _DEBUG
			if (AllocConsole())
			{
				freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
				freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
				freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

				std::cerr << "ErrorAttached\r\n";
			}
#endif
			return;
		}
		HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

		INPUT_RECORD inputRecords[256];
		DWORD eventsWritten;
		size_t len = command.length();
		for (size_t i = 0; i < len; ++i) {
			inputRecords[i].EventType = KEY_EVENT;
			inputRecords[i].Event.KeyEvent.bKeyDown = TRUE;
			inputRecords[i].Event.KeyEvent.dwControlKeyState = 0;
			inputRecords[i].Event.KeyEvent.uChar.UnicodeChar = command[i];
			inputRecords[i].Event.KeyEvent.wRepeatCount = 1;
			inputRecords[i].Event.KeyEvent.wVirtualKeyCode = 0;
			inputRecords[i].Event.KeyEvent.wVirtualScanCode = 0;
		}
		inputRecords[len].EventType = KEY_EVENT;
		inputRecords[len].Event.KeyEvent.bKeyDown = TRUE;
		inputRecords[len].Event.KeyEvent.dwControlKeyState = 0;
		inputRecords[len].Event.KeyEvent.uChar.UnicodeChar = '\r';
		inputRecords[len].Event.KeyEvent.wRepeatCount = 1;
		inputRecords[len].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
		inputRecords[len].Event.KeyEvent.wVirtualScanCode = 0;
		inputRecords[len + 1] = inputRecords[len];
		inputRecords[len + 1].Event.KeyEvent.bKeyDown = FALSE;
		if (!WriteConsoleInputW(hConsoleInput, inputRecords, len + 2, &eventsWritten)) {
			std::cerr << "Failed to write to console input buffer." << std::endl;
		}
		FreeConsole();
#ifdef _DEBUG
		if (AllocConsole())
		{
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
			freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

			std::cout << "Standard console restored." << std::endl;
		}
#endif
	}
} 