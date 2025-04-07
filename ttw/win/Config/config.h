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
    std::wstring bringToTop;
    int keySendCommand1 = 0;
    int keySendCommand2 = 0;
    int keySendCommandParameter1 = 0;
    int keySendCommandParameter2 = 0;
    int keyChooseFolder1 = 0;
    int keyChooseFolder2 = 0;
    int keyBringToTop1 = 0;
    int keyBringToTop2 = 0;
};
