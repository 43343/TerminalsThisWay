#include "chooseFolder.h"
#include <iostream>
#include <shlobj.h>
#include <thread>
#include "../Config/configManager.h"
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
std::wstring ChooseFolder::SelectFolder()
{
	isDialogOpen = true;
	const wchar_t CLASS_NAME[] = L"BrowserInfoWindowClass";

	WNDCLASSW wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;

	RegisterClassW(&wc);
	HWND hwndParent;

	hwndParent = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		CLASS_NAME,
		L"",
		WS_POPUP,
		0, 0, 200, 20,
		NULL,
		NULL, 
		GetModuleHandle(NULL), 
		NULL
	);
	CoInitialize(NULL);
	BROWSEINFOW bi = { 0 };
	std::wstring title = L"Select folder";
	std::wcout << "Выбранный путь: " << title << std::endl;
	printf("Выбранный путь:");
	bi.lpszTitle = title.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.hwndOwner = hwndParent;
	bi.lpfn = BrowseCallbackProc;

	LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

	std::wstring result;
	 
	if (pidl != 0)
	{
		
		wchar_t path[MAX_PATH];
		if (SHGetPathFromIDListW(pidl, path))
		{
			result = std::wstring(path);
		}
	}
	CoTaskMemFree(pidl);
	DestroyWindow(hwndParent);
	UnregisterClassW(CLASS_NAME, GetModuleHandle(NULL));
	CoUninitialize();
	std::wcout << "Выбранный путь: " << result << std::endl;
	isDialogOpen = false; 

	return result;
}

LRESULT CALLBACK ChooseFolder::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// static HWND hEdit;

	switch (uMsg) {
	case WM_CREATE:
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int CALLBACK ChooseFolder::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_SELCHANGED) {
		wchar_t path[MAX_PATH];
		if (SHGetPathFromIDListW((LPITEMIDLIST)lParam, path)) {
			SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE);
		}
		else {
			SendMessage(hwnd, BFFM_ENABLEOK, 0, FALSE);
		}
	}
	return 0;
}

LRESULT CALLBACK ChooseFolder::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (Simulating::IsSimulating())
		{
			return CallNextHookEx(g_ChooseFolderInstance->hHook, nCode, wParam, lParam);
		}
		if (ConfigManager::getInstance().getConfig().keyChooseFolder2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyChooseFolder1)
			{
				std::wstring folderPath;
				if (!g_ChooseFolderInstance->isDialogOpen)
				{
					folderPath = g_ChooseFolderInstance->SelectFolder();
				}
				if (!folderPath.empty())
				{
					g_ChooseFolderInstance->hTerminal.sendCommandToCMD(L"cd \"" + folderPath + L"\"", true);
					g_ChooseFolderInstance->hTerminal.sendCommandToCMD(L"cd /d \"" + folderPath + L"\"", true);
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
							std::wstring folderPath;
							if (!g_ChooseFolderInstance->isDialogOpen)
							{
								folderPath = g_ChooseFolderInstance->SelectFolder();
							}
							if (!folderPath.empty())
							{
								g_ChooseFolderInstance->hTerminal.sendCommandToCMD(L"cd \"" + folderPath + L"\"", true);
								g_ChooseFolderInstance->hTerminal.sendCommandToCMD(L"cd /d \"" + folderPath + L"\"", true);
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
					std::wstring folderPath;
					if (!g_ChooseFolderInstance->isDialogOpen)
					{
						folderPath = g_ChooseFolderInstance->SelectFolder();
					}
					if (!folderPath.empty())
					{
						g_ChooseFolderInstance->hTerminal.sendCommandToCMD(L"cd \"" + folderPath + L"\"", true);
						g_ChooseFolderInstance->hTerminal.sendCommandToCMD(L"cd /d \"" + folderPath + L"\"", true);
					}
				}
			}
		}
	}

	return CallNextHookEx(g_ChooseFolderInstance->hHook, nCode, wParam, lParam);
}