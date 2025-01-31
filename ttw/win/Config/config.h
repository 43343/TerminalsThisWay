#pragma once
#include <iostream>
#include <string>

class Config {
public:
    Config() {};
    std::wstring pathToTerminal;
    int launchByDefault;
    int runAsAdministrator;
    std::wstring sendCommand;
    std::wstring sendCommandParameter;
    std::wstring chooseFolder;
    int keySendCommand1 = 0;
    int keySendCommand2 = 0;
    int keySendCommandParameter1 = 0;
    int keySendCommandParameter2 = 0;
    int keyChooseFolder1 = 0;
    int keyChooseFolder2 = 0;
};
