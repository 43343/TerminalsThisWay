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
#include "getText.h"
#include <shlobj.h>
#include <cstdio>

HANDLE hMutex;

Application::Application(int argc, char* argv[]) {
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	if (argc > 1 && std::string(argv[1]) == "--update") {
		MessageBoxA(NULL, "The update was successfully installed", "Update is installed", MB_ICONINFORMATION | MB_OK);
	}

	char appDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
	{
		std::string newFolderPath = std::string(appDataPath) + "\\TerminalsThisWay";
		if (CreateDirectoryA(newFolderPath.c_str(), NULL)) {
			std::cout << "The folder was created successfully: " << newFolderPath << std::endl;
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			std::cout << "The folder already exists: " << newFolderPath << std::endl;
		}
		else {
			std::cerr << "Failed to create folder: " << newFolderPath << ", ошибка: " << GetLastError() << std::endl;
		}
		if (!SetCurrentDirectory((LPTSTR)newFolderPath.c_str())) {
			std::cerr << "Failed to set current directory. Error: " << GetLastError() << std::endl;
		}
	};


	ConfigManager::getInstance().generateConfigFile(L"config.conf");
	ConfigManager::getInstance().loadFromFile(L"config.conf");
	Config& config = ConfigManager::getInstance().getConfig();

	const char* mutexName = "TerminalsThisWayMutex";
	Sleep(1000);

	hMutex = CreateMutexA(NULL, FALSE, mutexName);

	if (hMutex == NULL) {
		std::cerr << "Failed to create mutex. Error: " << GetLastError() << std::endl;
		return;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		std::cout << "Another instance of the application is already running." << std::endl;
		MessageBoxA(NULL, "Another instance of the application is already running.", "Application already running", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
		CloseHandle(hMutex);
		return; 
	}
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	Terminal* cmdTerminal = new Terminal();
	if (config.runAsAdministrator && runAsAdministrator())
	{
		cmdTerminal->sendCommandToCMD(L"exit\r\n");
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
	GetText* getText = new GetText(cmdTerminal);
	ParameterInputWindow* parameterInputWindow = new ParameterInputWindow(cmdTerminal, hInstance, *getText);

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