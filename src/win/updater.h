#pragma once
#include <iostream>
#include <windows.h>
#include <winhttp.h>
#include <fstream>
#include <string>
#include "Window/modalWindow.h"

#pragma comment(lib, "winhttp.lib")

#define ID_BUTTON_YES 1
#define ID_BUTTON_NO  2

class Updater
{
public:
	Updater();
	~Updater();
	void checkUpdate();
	bool getActive();
private:
	void GetJsonData(std::string& dowloadUrl, std::string& lastVersion);
	bool DownloadFile(const std::string& url, const std::string& filePath);
	void RestartApplication();
	std::string GetJsonFromUrl(const std::wstring& url);
	bool isActive = false;
	ModalWindow* modalWindow;
};