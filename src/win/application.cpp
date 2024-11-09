#include "../application.h"
#include "Window/parameterInputWindow.h"
#include "Utility/utility.h"
#include "terminal.h"
#include "chooseFolder.h"
#include "Config/configManager.h"
#include "window/trayWindow.h"
#include <iostream>
#include "updater.h"
#include <windows.h>
#include "UIAutomation.h"
#include <shlobj.h>

HANDLE hMutex;

Application::Application() {

	char appDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
	{
		std::string newFolderPath = std::string(appDataPath) + "\\TerminalsThisWay";
		if (CreateDirectoryA(newFolderPath.c_str(), NULL)) {
			std::cout << "Папка успешно создана: " << newFolderPath << std::endl;
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			std::cout << "Папка уже существует: " << newFolderPath << std::endl;
		}
		else {
			std::cerr << "Не удалось создать папку: " << newFolderPath << ", ошибка: " << GetLastError() << std::endl;
		}
		if (!SetCurrentDirectory((LPTSTR)newFolderPath.c_str())) {
			std::cerr << "Failed to set current directory. Error: " << GetLastError() << std::endl;
		}
	};


	ConfigManager::getInstance().generateConfigFile("config.conf");
	ConfigManager::getInstance().loadFromFile("config.conf");
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
		return; // Завершаем приложение
	}
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	Terminal* cmdTerminal = new Terminal();
	if (config.runAsAdministrator && runAsAdministrator())
	{
		PostQuitMessage(0);
		cmdTerminal->sendCommandToCMD("exit\r\n");
	}

	std::cout << "PathToTerminal: " << config.pathToTerminal << std::endl;
	std::cout << "LaunchByDefault: " << config.launchByDefault << std::endl;
	std::cout << "RunAsAdministrator: " << config.runAsAdministrator << std::endl;
	std::cout << "SendCommand: " << config.sendCommand << std::endl;
	std::cout << "SendCommandParameter: " << config.sendCommandParameter << std::endl;
	std::cout << "ChooseFolder: " << config.chooseFolder << std::endl;
	TrayWindow* trayWindow = new TrayWindow(hInstance, cmdTerminal);
	ChooseFolder* chooseFolder = new ChooseFolder(cmdTerminal);
	UIAutomation* uiAutomation = new UIAutomation(cmdTerminal);
	ParameterInputWindow* parameterInputWindow = new ParameterInputWindow(cmdTerminal, hInstance, *uiAutomation);
	std::cout << "Press 'Insert' to get selected text..." << std::endl;

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