#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "config.h"

class ConfigManager {
public:
	static ConfigManager& getInstance();
	void generateConfigFile(const std::wstring& filePath);
    void overwritingConfig(const std::wstring& filePath, const Config& currentConfig);
    bool loadFromFile(const std::wstring& filePath);
    Config& getConfig();
private:
    Config config;
    void setKey(const std::wstring& hKey, int& hKey1, int& hKey2);
    ConfigManager() {}
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    bool setAutoLaunch(bool enable);
};
