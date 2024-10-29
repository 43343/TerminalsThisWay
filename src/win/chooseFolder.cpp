#include "chooseFolder.h"
#include <iostream>
#include <shlobj.h>
#include "Window/localization.h"
#include "Config/configManager.h"
ChooseFolder* g_ChooseFolderInstance = nullptr;
auto firstPressTimeChooseFolder = std::chrono::steady_clock::now();

ChooseFolder::ChooseFolder(Terminal* cmd) : hTerminal(*cmd)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_ChooseFolderInstance = this;
}
ChooseFolder::~ChooseFolder()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}
std::string ChooseFolder::SelectFolder()
{
	isDialogOpen = true; 

	BROWSEINFO bi = { 0 };
	Localization* local = new Localization(ConfigManager::getInstance().getConfig().language);
	std::string title = local->getText("chooseFolder");
	bi.lpszTitle = title.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; 

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	std::string result;

	if (pidl != 0)
	{
		
		char path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, path))
		{
			result = std::string(path);
		}
		CoTaskMemFree(pidl);
	}

	isDialogOpen = false; 

	return result;
}

LRESULT CALLBACK ChooseFolder::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (ConfigManager::getInstance().getConfig().keyChooseFolder2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder1)
			{
				std::string folderPath;
				if (!g_ChooseFolderInstance->isDialogOpen)
				{
					folderPath = g_ChooseFolderInstance->SelectFolder();
				}
				if (!folderPath.empty())
				{
					g_ChooseFolderInstance->hTerminal.sendCommandToCMD("cd \"" + folderPath + "\"\r\n");
					g_ChooseFolderInstance->hTerminal.sendCommandToCMD("cd /d \"" + folderPath + "\"\r\n");
				}
			}
		}
		else
		{
			if (ConfigManager::getInstance().getConfig().keyChooseFolder1 == ConfigManager::getInstance().getConfig().keyChooseFolder2)
			{
				if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder1)
				{
					auto currentTime = std::chrono::steady_clock::now();
					if (!g_ChooseFolderInstance->firstPressDetected) {
						g_ChooseFolderInstance->firstPressDetected = true;
						firstPressTimeChooseFolder = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTimeChooseFolder).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							std::string folderPath;
							if (!g_ChooseFolderInstance->isDialogOpen)
							{
								folderPath = g_ChooseFolderInstance->SelectFolder();
							}
							if (!folderPath.empty())
							{
								g_ChooseFolderInstance->hTerminal.sendCommandToCMD("cd \"" + folderPath + "\"\r\n");
								g_ChooseFolderInstance->hTerminal.sendCommandToCMD("cd /d \"" + folderPath + "\"\r\n");
							}
							g_ChooseFolderInstance->firstPressDetected = false;
						}
						else {
							firstPressTimeChooseFolder = currentTime; 
						}
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder1) { 
						g_ChooseFolderInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder2) { 
						g_ChooseFolderInstance->keyB_pressed = true;
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder1) {
						g_ChooseFolderInstance->keyA_pressed = false;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder2) {
						g_ChooseFolderInstance->keyB_pressed = false;
					}
				}

				std::cout << "gParameter1 " << g_ChooseFolderInstance->keyA_pressed << " gParameter2 " << g_ChooseFolderInstance->keyB_pressed;

				if (g_ChooseFolderInstance->keyA_pressed && g_ChooseFolderInstance->keyB_pressed) {
					std::string folderPath;
					if (!g_ChooseFolderInstance->isDialogOpen)
					{
						folderPath = g_ChooseFolderInstance->SelectFolder();
					}
					if (!folderPath.empty())
					{
						g_ChooseFolderInstance->hTerminal.sendCommandToCMD("cd \"" + folderPath + "\"\r\n");
						g_ChooseFolderInstance->hTerminal.sendCommandToCMD("cd /d \"" + folderPath + "\"\r\n");
					}
					// Âûïîëíèòå íóæíîå äåéñòâèå
				}
			}
		}
	}

	return CallNextHookEx(g_ChooseFolderInstance->hHook, nCode, wParam, lParam);
}