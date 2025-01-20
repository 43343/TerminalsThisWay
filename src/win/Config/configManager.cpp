#include "configManager.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>

#include "../Utility/utility.h"


ConfigManager& ConfigManager::getInstance() {
	static ConfigManager instance; 
	return instance;
}

void ConfigManager::generateConfigFile(const std::wstring& filePath)
{
	std::ifstream infile(filePath);
	if (infile.good()) {
		std::wcout << "Файл уже существует: " << filePath << std::endl;
		return;
	}

	
	std::ofstream outfile(filePath);
	if (outfile.is_open()) {
		outfile << "PathToTerminal=C:\\WINDOWS\\system32\\cmd.exe\n";
		outfile << "LaunchByDefault=0\n";
		outfile << "RunAsAdministrator=0\n";
		outfile << "SendCommand=Insert\n";
		outfile << "SendCommandParameter=NumLock\n";
		outfile << "ChooseFolder=Home\n";
		outfile.close();
		std::wcout << "Файл успешно создан: " << filePath << std::endl;
	}
	else {
		std::wcerr << "Не удалось открыть файл для записи: " << filePath << std::endl;
	}
}
void ConfigManager::overwritingConfig(const std::wstring& filePath, const Config& currentConfig)
{
	std::wofstream outfile(filePath);
	if (outfile.is_open()) {
		outfile << L"PathToTerminal=" + currentConfig.pathToTerminal + L"\n";
		outfile << L"LaunchByDefault=" + std::to_wstring(currentConfig.launchByDefault) + L"\n";
		outfile << L"RunAsAdministrator=" + std::to_wstring(currentConfig.runAsAdministrator) + L"\n";
		outfile << L"SendCommand=" + currentConfig.sendCommand + L"\n";
		outfile << L"SendCommandParameter=" + currentConfig.sendCommandParameter + L"\n";
		outfile << L"ChooseFolder=" + currentConfig.chooseFolder + L"\n";
		outfile.close();
		config = currentConfig;
		setKey(currentConfig.sendCommand, config.keySendCommand1, config.keySendCommand2);
		setKey(currentConfig.sendCommandParameter, config.keySendCommandParameter1, config.keySendCommandParameter2);
		setKey(currentConfig.chooseFolder, config.keyChooseFolder1, config.keyChooseFolder2);
		setAutoLaunch(currentConfig.launchByDefault);
	}
	else {
		std::wcerr << "Не удалось открыть файл для записи: " << filePath << std::endl;
	}
}
void ConfigManager::setKey(const std::wstring& hKey, int& hKey1, int& hKey2)
{
	size_t firstPlusPos = hKey.find('+');
	size_t secondPlusPos = hKey.find('+', firstPlusPos + 1);
	std::wstring key1;
	std::wstring key2;

	if (secondPlusPos != std::wstring::npos) {

		key1 = hKey.substr(0, secondPlusPos);
		key2 = hKey.substr(secondPlusPos + 1);

		
		key1.erase(key1.find_last_not_of(L" \t\n\r\f\v") + 1);
		key1.erase(0, key1.find_first_not_of(L" \t\n\r\f\v"));
		key2.erase(key2.find_last_not_of(L" \t\n\r\f\v") + 1);
		key2.erase(0, key2.find_first_not_of(L" \t\n\r\f\v"));

		hKey1 = stringTokey(key1);
		hKey2 = stringTokey(key2);
	}
	else if (firstPlusPos != std::wstring::npos) {
		key1 = hKey.substr(0, firstPlusPos);
		key2 = hKey.substr(firstPlusPos + 1);

		
		key1.erase(key1.find_last_not_of(L" \t\n\r\f\v") + 1);
		key1.erase(0, key1.find_first_not_of(L" \t\n\r\f\v"));
		key2.erase(key2.find_last_not_of(L" \t\n\r\f\v") + 1);
		key2.erase(0, key2.find_first_not_of(L" \t\n\r\f\v"));

		hKey1 = stringTokey(key1);
		hKey2 = stringTokey(key2);

	}
	else {
		key1 = hKey.substr(0, firstPlusPos);
		key1.erase(key1.find_last_not_of(L" \t\n\r\f\v") + 1);
		key1.erase(0, key1.find_first_not_of(L" \t\n\r\f\v"));
		hKey1 = stringTokey(key1);
	}

}
bool ConfigManager::loadFromFile(const std::wstring& filePath)
{
	std::wcout << getAbsolutePath(filePath);
	std::wifstream infile(getAbsolutePath(filePath));
	if (!infile.is_open()) {
		std::wcerr << "Íå óäàëîñü îòêðûòü ôàéë: " << filePath << std::endl;
		return false;
	}

	std::unordered_map<std::wstring, std::wstring> configMap;
	std::wstring line;
	while (std::getline(infile, line)) {
		auto delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos) {
			std::wstring key = line.substr(0, delimiterPos);
			std::wstring value = line.substr(delimiterPos + 1);
			configMap[key] = value;
		}
	}
	infile.close();

	config.pathToTerminal = configMap[L"PathToTerminal"];
	config.launchByDefault = std::stoi(configMap[L"LaunchByDefault"]);
	config.runAsAdministrator = std::stoi(configMap[L"RunAsAdministrator"]);
	config.sendCommand = configMap[L"SendCommand"];
	config.sendCommandParameter = configMap[L"SendCommandParameter"];
	config.chooseFolder = configMap[L"ChooseFolder"];
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

std::wstring ConfigManager::getAbsolutePath(const std::wstring& relativePath)
{
	return std::filesystem::absolute(relativePath).wstring();
}

Config& ConfigManager::getConfig()
{
	return config;
}