#include "terminal.h"
#include <windows.h>
#include <tlhelp32.h>
#include <codecvt>
#include <iostream>
#include "Config/configManager.h"
#include <io.h>
#include <fcntl.h>

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

void Terminal::createProcessCMD(const std::string& path)
{
	DWORD ThreadID;

	if (IsProcessRunning(startedProcessIDs)) {
		std::cout << "A terminal process is already running." << std::endl;
		return; // Terminal is already running, no need to start another
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
	PSTARTUPINFO.dwFlags |= STARTF_USESTDHANDLES;
	PSTARTUPINFO.dwFlags |= STARTF_USESHOWWINDOW;
	PSTARTUPINFO.wShowWindow = SW_SHOWNORMAL;
	std::wstring stemp = std::wstring(path.begin(), path.end());
	BOOL success = CreateProcessW(stemp.c_str(),
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
}

void Terminal::handle_cleanup(STARTUPINFOW startupinfo, PROCESS_INFORMATION  processinfo)
{
	if (!CloseHandle(startupinfo.hStdInput)) printf("[-] Could not close stdin handle\n");
	if (!CloseHandle(processinfo.hProcess)) printf("[-] Could not close process handle\n");
	if (!CloseHandle(processinfo.hThread)) printf("[-] Could not close thread handle\n");
}
void Terminal::sendCommandToCMD(const std::string& command)
{
	createProcessCMD(ConfigManager::getInstance().getConfig().pathToTerminal);
	WriteFile(PARENT_WRITE, command.c_str(), strlen(command.c_str()), NULL, NULL);
}