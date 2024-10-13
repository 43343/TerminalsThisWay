#include "../application.h"
#include "Window/parameterInputWindow.h"
#include "Utility/utility.h"
#include "terminal.h"
#include "chooseFolder.h"
#include "Config/configManager.h"
#include "window/trayWindow.h"
#include <iostream>
#include <windows.h>
#include "UIAutomation.h"
#include "Window/localization.h"

HANDLE hMutex;

Application::Application() {

    char path[MAX_PATH];
    if (GetModuleFileName(NULL, path, MAX_PATH) == 0) {
        std::cerr << "Failed to get module file name. Error: " << GetLastError() << std::endl;
        return;
    }

    std::string szPath = path;
    std::string szDir = szPath.substr(0, szPath.rfind("\\") + 1);
    std::cout << "Attempting to set current directory to: " << path << std::endl;
    if (!SetCurrentDirectory((LPTSTR)szDir.c_str())) {
        std::cerr << "Failed to set current directory. Error: " << GetLastError() << std::endl;
    }
      

    ConfigManager::getInstance().generateConfigFile("config.conf");
    ConfigManager::getInstance().loadFromFile("config.conf");
    Config& config = ConfigManager::getInstance().getConfig();

    const char* mutexName = "TerminalsThisWayMutex";

    hMutex = CreateMutexA(NULL, FALSE, mutexName);

    if (hMutex == NULL) {
        std::cerr << "Failed to create mutex. Error: " << GetLastError() << std::endl;
        return;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Localization* local = new Localization(config.language);
        std::cout << "Another instance of the application is already running." << std::endl;
        MessageBoxA(NULL, local->getText("applicationAlreadyRunningDescription").c_str(), local->getText("applicationAlreadyRunning").c_str(), MB_ICONEXCLAMATION | MB_OK);
        PostQuitMessage(0);
        CloseHandle(hMutex);
        return; // Завершаем приложение
    }

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    Terminal* cmdTerminal = new Terminal();
    cmdTerminal->createProcessCMD(ConfigManager::getInstance().getConfig().pathToTerminal);
    if (config.runAsAdministrator && runAsAdministrator())
    {
        PostQuitMessage(0);
        cmdTerminal->sendCommandToCMD("exit\r\n");
    }

    std::cout << "PathToTerminal: " << config.pathToTerminal << std::endl;
    std::cout << "Language: " << config.language << std::endl;
    std::cout << "LaunchByDefault: " << config.launchByDefault << std::endl;
    std::cout << "RunAsAdministrator: " << config.runAsAdministrator << std::endl;
    std::cout << "SendCommand: " << config.sendCommand << std::endl;
    std::cout << "SendCommandParameter: " << config.sendCommandParameter << std::endl;
    std::cout << "ChooseFolder: " << config.chooseFolder << std::endl;
    TrayWindow* trayWindow = new TrayWindow(hInstance, cmdTerminal);
    ChooseFolder* chooseFolder = new ChooseFolder(cmdTerminal);
    UIAutomation* uiAutomation = new UIAutomation(cmdTerminal);
    ParameterInputWindow* parameterInputWindow = new ParameterInputWindow(cmdTerminal, hInstance, uiAutomation);
    std::cout << "Press 'Insert' to get selected text..." << std::endl;
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