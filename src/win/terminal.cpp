#include "terminal.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "Config/configManager.h"

Terminal::Terminal() 
{
	workerThread = std::thread(&Terminal::commandProcessingLoop, this);
}

bool Terminal::IsProcessRunning(DWORD processID) 
{
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
		Sleep(1000);
		startedProcessIDs = 0;
	}
	currentPathTerminal = pathToTerminalQuotes;
	if (IsProcessRunning(startedProcessIDs) && startedProcessIDs != 0) {
		std::cout << "A terminal process is already running." << std::endl;
		return;
	}

	STARTUPINFOW PSTARTUPINFO = { 0 };
	PROCESS_INFORMATION PPROCESSINFO = { 0 };
	SECURITY_ATTRIBUTES SECURITYATTR;
	BOOL success = CreateProcessW(pathToTerminalQuotes.c_str(),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&PSTARTUPINFO,
		&PPROCESSINFO);

	startedProcessIDs = PPROCESSINFO.dwProcessId;

	WaitForSingleObject(PPROCESSINFO.hProcess, 1000);
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
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		commandQueue.push(command);
	}
	cv.notify_one();
} 
void Terminal::commandProcessingLoop() {
	while (true) {
		std::unique_lock<std::mutex> lock(queueMutex);

		cv.wait(lock, [this]() { return !commandQueue.empty(); });

		if (commandQueue.empty()) {
			break;
		}

		std::wstring nextCommand = commandQueue.front();
		commandQueue.pop();
		lock.unlock(); 

		processNextCommand(nextCommand); 

		lock.lock();
	}
}
void Terminal::processNextCommand(const std::wstring& command) {

	if (IsProcessRunning(startedProcessIDs) && startedProcessIDs != 0) {
		
		FreeConsole();
		if (!AttachConsole(startedProcessIDs)) {
			MessageBoxA(NULL, "Couldn't connect to the terminal.", "Error", MB_ICONERROR | MB_OK);
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
		if (!SetConsoleCtrlHandler(IgnoreCtrlHandler, TRUE)) {
			MessageBoxA(NULL, "Failed to set control handler.", "Error", MB_ICONERROR | MB_OK);
			return;
		}
		if (!WriteConsoleInputW(hConsoleInput, inputRecords, len + 2, &eventsWritten)) {
			std::cerr << "Failed to write to console input buffer." << std::endl;
		}
		while (!isTerminalReady() && IsProcessRunning(startedProcessIDs)) {
			Sleep(100);
		}
		if (!IsProcessRunning(startedProcessIDs)) {
			commandQueue = {};
		}
	}
}

bool Terminal::isTerminalReady() {
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsoleOutput == INVALID_HANDLE_VALUE) {
		MessageBoxA(NULL, "Invalid console output handle.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	CHAR buffer[4096];
	DWORD charsRead = 0;

	COORD coord = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
		MessageBoxA(NULL, "GetConsoleScreenBufferInfo.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	DWORD consoleSize = std::min<DWORD>(csbi.dwSize.X * csbi.dwSize.Y, 4096);

	if (!ReadConsoleOutputCharacterA(hConsoleOutput, buffer, consoleSize, coord, &charsRead)) {
		MessageBoxA(NULL, "ReadConsoleOutputCharacterW.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	FreeConsole();
	std::string output(buffer, charsRead);
	return output.find(">") != std::string::npos;
}

BOOL WINAPI Terminal::IgnoreCtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		std::cout << "Control-C event detected! Performing cleanup..." << std::endl;
		return TRUE;

	case CTRL_BREAK_EVENT:
		std::cout << "Control-Break event detected!" << std::endl;
		return TRUE;

	case CTRL_CLOSE_EVENT:
		std::cout << "Console window closing event detected!" << std::endl;
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		std::cout << "User is logging off!" << std::endl;
		return TRUE;

	case CTRL_SHUTDOWN_EVENT:
		std::cout << "System is shutting down!" << std::endl;
		return TRUE;

	default:
		return FALSE;
	}
}