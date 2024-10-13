#include "configManager.h"
#include <string>
#include <algorithm>
#include <cctype>

#include "../Utility/utility.h"


ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance; // Локальная статическая переменная
    return instance;
}

void ConfigManager::generateConfigFile(const std::string& filePath)
{
    std::ifstream infile(filePath);
    if (infile.good()) {
        std::cout << "Файл уже существует: " << filePath << std::endl;
        return;
    }

    // Если файл не существует, создаем и записываем в него данные
    std::ofstream outfile(filePath);
    if (outfile.is_open()) {
        outfile << "PathToTerminal=C:\\Program Files\\PowerShell\\7\\pwsh.exe\n";
        outfile << "Language=English\n";
        outfile << "LaunchByDefault=0\n";
        outfile << "RunAsAdministrator=0\n";
        outfile << "SendCommand=Insert\n";
        outfile << "SendCommandParameter=NumLock\n";
        outfile << "ChooseFolder=Home\n";
        outfile.close();
        std::cout << "Файл успешно создан: " << filePath << std::endl;
    }
    else {
        std::cerr << "Не удалось открыть файл для записи: " << filePath << std::endl;
    }
}
void ConfigManager::overwritingConfig(const std::string& filePath, const Config& currentConfig)
{
    std::ofstream outfile(filePath);
    if (outfile.is_open()) {
        outfile << "PathToTerminal=" + currentConfig.pathToTerminal + "\n";
        outfile << "Language=" + currentConfig.language + "\n";
        outfile << "LaunchByDefault=" + std::to_string(currentConfig.launchByDefault) + "\n";
        outfile << "RunAsAdministrator=" + std::to_string(currentConfig.runAsAdministrator) + "\n";
        outfile << "SendCommand=" + currentConfig.sendCommand + "\n";
        outfile << "SendCommandParameter=" + currentConfig.sendCommandParameter + "\n";
        outfile << "ChooseFolder=" + currentConfig.chooseFolder + "\n";
        outfile.close();
        std::cout << "Файл успешно создан: " << filePath << std::endl;
        config = currentConfig;
        setKey(currentConfig.sendCommand,config.keySendCommand1, config.keySendCommand2);
        setKey(currentConfig.sendCommandParameter, config.keySendCommandParameter1, config.keySendCommandParameter2);
        setKey(currentConfig.chooseFolder, config.keyChooseFolder1, config.keyChooseFolder2);
        setAutoLaunch(currentConfig.launchByDefault);
    }
    else {
        std::cerr << "Не удалось открыть файл для записи: " << filePath << std::endl;
    }
}
void ConfigManager::setKey(const std::string& hKey, int& hKey1, int& hKey2)
{
    size_t firstPlusPos = hKey.find('+');
    size_t secondPlusPos = hKey.find('+', firstPlusPos + 1);
    std::string key1;
    std::string key2;

    if (secondPlusPos != std::string::npos) {

        key1 = hKey.substr(0, secondPlusPos);
        key2 = hKey.substr(secondPlusPos + 1);

        // Убираем пробелы в начале и конце строк (если необходимо)
        key1.erase(key1.find_last_not_of(" \t\n\r\f\v") + 1);
        key1.erase(0, key1.find_first_not_of(" \t\n\r\f\v"));
        key2.erase(key2.find_last_not_of(" \t\n\r\f\v") + 1);
        key2.erase(0, key2.find_first_not_of(" \t\n\r\f\v"));

        hKey1 = stringTokey(key1);
        hKey2 = stringTokey(key2);
    }
    else if (firstPlusPos != std::string::npos) {
        key1 = hKey.substr(0, firstPlusPos);
        key2 = hKey.substr(firstPlusPos + 1);

        // Убираем пробелы в начале и конце строк (если необходимо)
        key1.erase(key1.find_last_not_of(" \t\n\r\f\v") + 1);
        key1.erase(0, key1.find_first_not_of(" \t\n\r\f\v"));
        key2.erase(key2.find_last_not_of(" \t\n\r\f\v") + 1);
        key2.erase(0, key2.find_first_not_of(" \t\n\r\f\v"));

        hKey1 = stringTokey(key1);
        hKey2 = stringTokey(key2);

    }
    else {
        key1 = hKey.substr(0, firstPlusPos);
        key1.erase(key1.find_last_not_of(" \t\n\r\f\v") + 1);
        key1.erase(0, key1.find_first_not_of(" \t\n\r\f\v"));
        hKey1 = stringTokey(key1);
    }

}
bool ConfigManager::loadFromFile(const std::string& filePath)
{
    std::cout << getAbsolutePath(filePath);
    std::ifstream infile(getAbsolutePath(filePath));
    if (!infile.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
        return false;
    }

    std::unordered_map<std::string, std::string> configMap;
    std::string line;
    while (std::getline(infile, line)) {
        auto delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            configMap[key] = value;
        }
    }
    infile.close();

    config.pathToTerminal = configMap["PathToTerminal"];
    config.language = configMap["Language"];
    config.launchByDefault = std::stoi(configMap["LaunchByDefault"]);
    config.runAsAdministrator = std::stoi(configMap["RunAsAdministrator"]);
    config.sendCommand = configMap["SendCommand"];
    config.sendCommandParameter = configMap["SendCommandParameter"];
    config.chooseFolder = configMap["ChooseFolder"];
    setKey(config.sendCommand, config.keySendCommand1, config.keySendCommand2);
    setKey(config.sendCommandParameter, config.keySendCommandParameter1, config.keySendCommandParameter2);
    setKey(config.chooseFolder, config.keyChooseFolder1, config.keyChooseFolder2);
    setAutoLaunch(config.launchByDefault);

    return true;
}

bool ConfigManager::setAutoLaunch(bool enable)
{
    char path[MAX_PATH];
    if (GetModuleFileName(NULL, path, MAX_PATH) == 0) {
        std::cerr << "Failed to get module file name." << std::endl;
        return false;
    }
    std::cout << path;
    const char* appName = "TerminalsThisWay";
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_WRITE, &hKey);
    std::cout << "Result of RegOpenKeyEx: " << result << std::endl;
    if (result != ERROR_SUCCESS) {
        std::cerr << "Failed to open registry key. Error code: " << GetLastError() << std::endl;
        return false;
    }
    if (enable) {
        // Добавляем программу в автозапуск
        result = RegSetValueEx(hKey, appName, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(path),
            static_cast<DWORD>((strlen(path) + 1) * sizeof(char)));
        if (result != ERROR_SUCCESS) {
            std::cerr << "Failed to set registry value." << std::endl;
            RegCloseKey(hKey);
            return false;
        }
    } 
    else {
        // Удаляем программу из автозапуска
        result = RegDeleteValue(hKey, appName);
        if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
            std::cerr << "Failed to delete registry value." << std::endl;
            RegCloseKey(hKey);
            return false;
        }
    }
    RegCloseKey(hKey);
    return true;
}

std::string ConfigManager::getAbsolutePath(const std::string& relativePath)
{
    return std::filesystem::absolute(relativePath).string();
}

Config& ConfigManager::getConfig()
{
    return config;
}