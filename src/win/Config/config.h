#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

class Config {
public:
    Config() {};
    std::string pathToTerminal;
    int launchByDefault;
    int runAsAdministrator;
    std::string sendCommand;
    std::string sendCommandParameter;
    std::string chooseFolder;
    int keySendCommand1 = 0;
    int keySendCommand2 = 0;
    int keySendCommandParameter1 = 0;
    int keySendCommandParameter2 = 0;
    int keyChooseFolder1 = 0;
    int keyChooseFolder2 = 0;
};