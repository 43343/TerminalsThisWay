#include "terminal.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <sstream>
#include <random>
#include <psapi.h>
#include <filesystem>
#include "Config/configManager.h"
#include "Utility/utility.h"

namespace fs = std::filesystem;

Terminal::Terminal() {}
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
	if (currentPathTerminal != pathToTerminalQuotes && startedProcessIDsCMD != 0)
	{
		sendCommandToCMD(L"exit", false);
		Sleep(1000);
		startedProcessIDsCMD = 0;
	}
	currentPathTerminal = pathToTerminalQuotes;
	if (IsProcessRunning(startedProcessIDsCMD) && startedProcessIDsCMD != 0) {
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

	startedProcessIDsCMD = PPROCESSINFO.dwProcessId;
	WaitForSingleObject(PPROCESSINFO.hProcess, 2000);
}
void Terminal::createProcessCommand()
{
	if (IsProcessRunning(startedProcessIDsCommand) && startedProcessIDsCommand != 0) {
		std::cout << "A terminal process is already running." << std::endl;
		return;
	}
	std::wstring token = generateToken();
	std::wstring commandLine = L"Command.exe " + token;
	STARTUPINFOW PSTARTUPINFO = { 0 };
	PROCESS_INFORMATION PPROCESSINFO = { 0 };
	SECURITY_ATTRIBUTES SECURITYATTR;
	BOOL success = CreateProcessW(NULL,
		&commandLine[0],
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&PSTARTUPINFO,
		&PPROCESSINFO);
	startedProcessIDsCommand = PPROCESSINFO.dwProcessId;
}
void Terminal::sendCommandToCMD(const std::wstring& command, const bool& createCmd)
{
	if (createCmd)
	{
		createProcessCMD(ConfigManager::getInstance().getConfig().pathToTerminal);
	}
	createProcessCommand();
	const wchar_t* SHARED_MEMORY_NAME = L"TTWMemory";
	const size_t SHARED_MEMORY_SIZE = 6800;
	HANDLE hMapFile = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		SHARED_MEMORY_SIZE,
		SHARED_MEMORY_NAME
	);
	if (hMapFile == NULL) {
		std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
	}
	LPVOID pBuf = MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		SHARED_MEMORY_SIZE
	);
	if (pBuf == NULL) {
		std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
		CloseHandle(hMapFile);
	}
	//MessageBoxW(NULL, L"reinterpret_cast<wchar_t>(pBuf)", L"Application already running", MB_ICONEXCLAMATION | MB_OK);
	std::wstring data = std::to_wstring(startedProcessIDsCMD) + L"|" + command;
	memcpy(pBuf, data.c_str(), (data.size() + 1) * sizeof(wchar_t));
	while (wcslen(static_cast<wchar_t*>(pBuf)) > 0)
	{
		createProcessCommand();
		Sleep(50);
	}
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
}
BOOL CALLBACK Terminal::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	DWORD processId = 0;
	GetWindowThreadProcessId(hwnd, &processId);
	struct Params {
		DWORD targetProcessId;
		HWND* pTargetHwnd;
	};
	Params* params = reinterpret_cast<Params*>(lParam);

	if (processId == params->targetProcessId && IsWindowVisible(hwnd)) {
		*(params->pTargetHwnd) = hwnd;
		return FALSE;
	}
	return TRUE;
}

void Terminal::bringProcessWindowToTop() {
	HWND hTargetWnd = nullptr;
	struct Params {
		DWORD targetProcessId;
		HWND* pTargetHwnd;
	} params = { startedProcessIDsCMD, &hTargetWnd };

	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&params));
	if (hTargetWnd == nullptr) {
		std::cout << "Window is not found";
		return;
	}
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hTargetWnd, &wp);

	RECT rect;

	if (wp.showCmd == SW_SHOWMINIMIZED || !GetWindowRect(hTargetWnd, &rect) || rect.right <= rect.left || rect.bottom <= rect.top) {
		wp.showCmd = SW_RESTORE;
		SetWindowPlacement(hTargetWnd, &wp);
		Sleep(50);

		PostMessage(hTargetWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		Sleep(50);

		ShowWindow(hTargetWnd, SW_RESTORE);
		ShowWindow(hTargetWnd, SW_SHOWNA);
		Sleep(50);

		GetWindowPlacement(hTargetWnd, &wp);
		GetWindowRect(hTargetWnd, &rect);
	}
	if (GetForegroundWindow() != hTargetWnd) {
		DWORD foreThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
		DWORD targetThread = GetWindowThreadProcessId(hTargetWnd, NULL);

		if (foreThread != targetThread) {
			AttachThreadInput(foreThread, targetThread, TRUE);
			BringWindowToTop(hTargetWnd);
			SetWindowPos(hTargetWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			SetForegroundWindow(hTargetWnd);
			SetActiveWindow(hTargetWnd);
			SetFocus(hTargetWnd);

			AttachThreadInput(foreThread, targetThread, FALSE);
		}
		else {
			BringWindowToTop(hTargetWnd);
			SetWindowPos(hTargetWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			SetForegroundWindow(hTargetWnd);
			SetActiveWindow(hTargetWnd);
			SetFocus(hTargetWnd);
		}
		if (GetForegroundWindow() != hTargetWnd) {
			keybd_event(VK_MENU, 0, 0, 0);
			keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
			SetForegroundWindow(hTargetWnd);
		}
	}
	SetFocus(NULL);
}
std::wstring Terminal::generateToken()
{
	std::wstringstream ss;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 15);
	for (int i = 0; i < 32; ++i) {
		ss << std::hex << dis(gen);
	}
	std::wofstream outfile{ fs::path(getAppdataFolder() + L"\\temp.txt") };
	if (outfile.is_open()) {
		outfile << ss.str();
		outfile.close();
	}
	return ss.str();
}