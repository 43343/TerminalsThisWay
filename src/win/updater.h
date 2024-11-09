#pragma once
#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <fstream>
#include <string>
#include "Window/modalWindow.h"

#define ID_BUTTON_YES 1
#define ID_BUTTON_NO  2

class Updater
{
public:
	Updater();
	~Updater();
	bool DownloadFile(const std::string& url, const std::string& localPath);
	std::string GetLatestVersion();
	void RestartApplication();
	void checkUpdate();
	bool getActive();
private:
	bool isActive = false;
	ModalWindow* modalWindow;
};