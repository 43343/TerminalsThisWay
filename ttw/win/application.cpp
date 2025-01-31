#include "../application.h"
#include "Window/parameterInputWindow.h"
#include "Utility/utility.h"
#include "terminal.h"
#include "Window/chooseFolder.h"
#include "Config/configManager.h"
#include "Window/trayWindow.h"
#include <iostream>
#include "updater.h"
#include <windows.h>
#include "getHighlightedText.h"
#include <shlobj.h>

HANDLE hMutex;

Application::Application(int argc, char* argv[]) {
	if (argc > 1 && std::string(argv[1]) == "--update") {
		MessageBoxA(NULL, "The update was successfully installed", "Update is installed", MB_ICONINFORMATION | MB_OK);
	}
	wchar_t exePath[MAX_PATH];
	if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
		std::wstring exeDir = exePath;
		size_t lastSlashPos = exeDir.find_last_of(L"\\");
		if (lastSlashPos != std::wstring::npos) {
			exeDir = exeDir.substr(0, lastSlashPos); 
			if (!SetCurrentDirectoryW(exeDir.c_str())) {
				std::cerr << "Failed to set current directory to executable path. Error: " << GetLastError() << std::endl;
			}
			else {
				std::wcout << "Current directory set to: " << exeDir << std::endl;
			}
		}
	}
	else {
		std::wcerr << "Failed to get executable path. Error: " << GetLastError() << std::endl;
	}


	ConfigManager::getInstance().generateConfigFile(getAppdataFolder() + L"\\config.conf");
	ConfigManager::getInstance().loadFromFile(getAppdataFolder() + L"\\config.conf");
	Config& config = ConfigManager::getInstance().getConfig();

	const char* mutexName = "TerminalsThisWayMutex";
	Sleep(1000);

	hMutex = CreateMutexA(NULL, FALSE, mutexName);

	if (hMutex == NULL) {
		std::cerr << "Failed to create mutex. Error: " << GetLastError() << std::endl;
		return;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBoxA(NULL, "Another instance of the application is already running.", "Application already running", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
		CloseHandle(hMutex);
		return; 
	}
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	Terminal* cmdTerminal = new Terminal();
	if (config.runAsAdministrator && runAsAdministrator())
	{
		cmdTerminal->sendCommandToCMD(L"exit", false);
		PostQuitMessage(0);
	}

	if (std::remove("TerminalsThisWayNew.exe") == 0)
	{
		std::cout << "The file was successfully deleted";
	}
	else
	{
		std::cout << "Error deleting a file";
	}


	std::wcout << "PathToTerminal: " << config.pathToTerminal << std::endl;
	std::wcout << "LaunchByDefault: " << config.launchByDefault << std::endl;
	std::wcout << "RunAsAdministrator: " << config.runAsAdministrator << std::endl;
	std::wcout << "SendCommand: " << config.sendCommand << std::endl;
	std::wcout << "SendCommandParameter: " << config.sendCommandParameter << std::endl;
	std::wcout << "ChooseFolder: " << config.chooseFolder << std::endl;
	TrayWindow* trayWindow = new TrayWindow(hInstance, cmdTerminal);
	ChooseFolder* chooseFolder = new ChooseFolder(cmdTerminal);
	GetHighlitedText* getText = new GetHighlitedText(cmdTerminal);
	ParameterInputWindow* parameterInputWindow = new ParameterInputWindow(cmdTerminal, hInstance);

}
Application::~Application()
{

}

int Application::Run() {
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CloseHandle(hMutex);
	return static_cast<int>(msg.wParam);
}