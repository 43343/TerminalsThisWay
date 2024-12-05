#include "terminal.h"
#include <windows.h>
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
	if (currentPathTerminal != path && startedProcessIDs != 0)
	{
		startedProcessIDs = 0;
		std::string exitCommand = "exit\n";
		WriteFile(PARENT_WRITE, exitCommand.c_str(), strlen(exitCommand.c_str()), NULL, NULL);
	}
	currentPathTerminal = path;
	if (IsProcessRunning(startedProcessIDs) && startedProcessIDs != 0) {
		std::cout << "A terminal process is already running." << std::endl;
		return; 
	}

	STARTUPINFOW PSTARTUPINFO;
	PROCESS_INFORMATION PPROCESSINFO;
	SECURITY_ATTRIBUTES SECURITYATTR;

	SECURITYATTR.nLength = sizeof(SECURITY_ATTRIBUTES);
	SECURITYATTR.bInheritHandle = TRUE;
	SECURITYATTR.lpSecurityDescriptor = NULL;

	CreatePipe(&CMD_READ, &PARENT_WRITE, &SECURITYATTR, 0);

	ZeroMemory(&PSTARTUPINFO, sizeof(STARTUPINFO));

	PSTARTUPINFO.cb = sizeof(STARTUPINFO);
	PSTARTUPINFO.hStdInput = CMD_READ;
	PSTARTUPINFO.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	PSTARTUPINFO.wShowWindow = SW_SHOWNORMAL;
	BOOL success = CreateProcessW(path.c_str(),
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

	//WaitForSingleObject(hProcessCMD, INFINITE);

	handle_cleanup(PSTARTUPINFO, PPROCESSINFO);
	std::string command = "chcp 65001\r\n";
	DWORD bytesToWrite = static_cast<DWORD>(command.size());
	WriteFile(PARENT_WRITE, command.c_str(), bytesToWrite, NULL, NULL);
}

void Terminal::handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo)
{
	if (!CloseHandle(startupinfo.hStdInput)) printf("[-] Could not close stdin handle\n");
	if (!CloseHandle(processinfo.hProcess)) printf("[-] Could not close process handle\n");
	if (!CloseHandle(processinfo.hThread)) printf("[-] Could not close thread handle\n");
}
std::string Terminal::utf16ToUtf8(const std::wstring& utf16Str)
{
	if (utf16Str.empty()) {
		return std::string();
	}

	
	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (utf8Size <= 0) {
		throw std::runtime_error("Failed to calculate UTF-8 string size.");
	}

	
	std::string utf8Str(utf8Size - 1, '0'); 
	WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);

	return utf8Str;
}
void Terminal::sendCommandToCMD(const std::wstring& command)
{
	createProcessCMD(ConfigManager::getInstance().getConfig().pathToTerminal);
	std::string utf8Command = utf16ToUtf8(command);
	DWORD bytesToWrite = static_cast<DWORD>(utf8Command.size());

	DWORD bytesWritten = 0;

	BOOL result = WriteFile(PARENT_WRITE, utf8Command.c_str(), bytesToWrite, &bytesWritten, NULL);
	if (!result || bytesWritten != bytesToWrite) {
		throw std::runtime_error("Failed to write command to CMD.");
	}
} 