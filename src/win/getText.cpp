#include "getText.h"
#include <iostream>
#include "Config/configManager.h"

GetText* g_GetTextInstance = nullptr;
auto firstPressTime = std::chrono::steady_clock::now();

GetText::GetText(Terminal* cmd) : hTerminal(cmd)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_GetTextInstance = this;
}
GetText::~GetText()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}
std::wstring GetText::GetClipboardText() {
	if (!OpenClipboard(nullptr)) {
		return L""; 
	}

	HANDLE hData = GetClipboardData(CF_UNICODETEXT); 
	if (hData == nullptr) {
		CloseClipboard();
		return L""; 
	}

	wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData)); 
	if (pszText == nullptr) {
		CloseClipboard();
		return L""; 
	}

	GlobalUnlock(hData); 
	CloseClipboard();    

	return std::wstring(pszText);
}
void GetText::SetClipboardText(const std::wstring& text) {
	if (!OpenClipboard(nullptr)) {
		return; 
	}

	EmptyClipboard();

	
	size_t sizeInBytes = (text.size() + 1) * sizeof(wchar_t);

	
	HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
	if (hGlob != nullptr) {
		void* pGlobMem = GlobalLock(hGlob); 
		if (pGlobMem != nullptr) {
			memcpy(pGlobMem, text.c_str(), sizeInBytes); 
			GlobalUnlock(hGlob);                        
			SetClipboardData(CF_UNICODETEXT, hGlob);    
		}
		else {
			GlobalFree(hGlob); 
		}
	}

	CloseClipboard();
}
void GetText::SimulateCtrlC() {
	INPUT inputs[4] = {};
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CONTROL;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = 'C';

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'C';
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_CONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(4, inputs, sizeof(INPUT));
}
std::wstring GetText::getSelectedText()
{
	std::wstring originalClipboardContent = GetClipboardText();
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		CloseClipboard();
	}
	SimulateCtrlC(); 
	Sleep(100);
	std::wstring selectedText = GetClipboardText();
	if (selectedText.empty()) {
		SetClipboardText(originalClipboardContent);
		return L"";
	}
	size_t start = selectedText.find_first_not_of(L" \t");
	size_t end = selectedText.find_last_not_of(L" \t");

	if (start != std::wstring::npos && end != std::wstring::npos) {
		selectedText = selectedText.substr(start, end - start + 1);
	}
	else if (start != std::wstring::npos) {
		selectedText = selectedText.substr(start);
	}
	else {
		selectedText = L"";
	}
	SetClipboardText(originalClipboardContent);
	return selectedText;
}
LRESULT CALLBACK GetText::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		std::cout << "key " << ConfigManager::getInstance().getConfig().keySendCommand2;
		if (ConfigManager::getInstance().getConfig().keySendCommand2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1)
			{
				std::wstring command = g_GetTextInstance->getSelectedText();
				if (!command.empty())
				{
					g_GetTextInstance->hTerminal->sendCommandToCMD(command, true);
				}
			}
		}
		else
		{
			if (ConfigManager::getInstance().getConfig().keySendCommand1 == ConfigManager::getInstance().getConfig().keySendCommand2)
			{
				if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1)
				{
					auto currentTime = std::chrono::steady_clock::now();
					if (!g_GetTextInstance->firstPressDetected) {
						g_GetTextInstance->firstPressDetected = true;
						firstPressTime = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTime).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							std::wstring command = g_GetTextInstance->getSelectedText();
							if (!command.empty())
							{
								g_GetTextInstance->hTerminal->sendCommandToCMD(command, true);
							}
							g_GetTextInstance->firstPressDetected = false;
						}
						else {
							firstPressTime = currentTime; 
						}
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1) { 
						g_GetTextInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand2) { 
						g_GetTextInstance->keyB_pressed = true;
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1) {
						g_GetTextInstance->keyA_pressed = false;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand2) {
						g_GetTextInstance->keyB_pressed = false;
					}
				}

				if (g_GetTextInstance->keyA_pressed && g_GetTextInstance->keyB_pressed) {
					std::wstring command = g_GetTextInstance->getSelectedText();
					if (!command.empty())
					{
						g_GetTextInstance->hTerminal->sendCommandToCMD(command, true);
					}
				}
			}
		}
	}

	return CallNextHookEx(g_GetTextInstance->hHook, nCode, wParam, lParam);
}