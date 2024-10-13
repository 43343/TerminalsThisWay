#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "config.h"
#include <filesystem>

class ConfigManager {
public:
	static ConfigManager& getInstance();
	void generateConfigFile(const std::string& filePath);
    void overwritingConfig(const std::string& filePath, const Config& currentConfig);
    bool loadFromFile(const std::string& filePath);
    Config& getConfig();
private:
    Config config;
    void setKey(const std::string& hKey, int& hKey1, int& hKey2);
    ConfigManager() {}
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    bool setAutoLaunch(bool enable);
    std::string getAbsolutePath(const std::string& relativePath);
};