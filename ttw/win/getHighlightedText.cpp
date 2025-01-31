#include "getHighlightedText.h"
#include "Config/configManager.h"
#include <iostream>

GetHighlitedText* g_GetHighlitedTextInstance = nullptr;
auto firstPressTimeHighlitedText = std::chrono::steady_clock::now();

GetHighlitedText::GetHighlitedText(Terminal* cmd) : hTerminal(cmd)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_GetHighlitedTextInstance = this;
}
GetHighlitedText::~GetHighlitedText()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}
LRESULT CALLBACK GetHighlitedText::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (Simulating::IsSimulating())
		{
			return CallNextHookEx(g_GetHighlitedTextInstance->hHook, nCode, wParam, lParam);
		}
		if (ConfigManager::getInstance().getConfig().keySendCommand2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1)
			{
				std::wstring command = g_GetHighlitedTextInstance->getSelectedText();
				if (!command.empty())
				{
					g_GetHighlitedTextInstance->hTerminal->sendCommandToCMD(command, true);
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
					if (!g_GetHighlitedTextInstance->firstPressDetected) {
						g_GetHighlitedTextInstance->firstPressDetected = true;
						firstPressTimeHighlitedText = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTimeHighlitedText).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							std::wstring command = g_GetHighlitedTextInstance->getSelectedText();
							if (!command.empty())
							{
								g_GetHighlitedTextInstance->hTerminal->sendCommandToCMD(command, true);
							}
							g_GetHighlitedTextInstance->firstPressDetected = false;
						}
						else {
							firstPressTimeHighlitedText = currentTime;
						}
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1) {
						g_GetHighlitedTextInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand2) {
						g_GetHighlitedTextInstance->keyB_pressed = true;
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1) {
						g_GetHighlitedTextInstance->keyA_pressed = false;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand2) {
						g_GetHighlitedTextInstance->keyB_pressed = false;
					}
				}

				if (g_GetHighlitedTextInstance->keyA_pressed && g_GetHighlitedTextInstance->keyB_pressed) {
					std::wstring command = g_GetHighlitedTextInstance->getSelectedText();
					if (!command.empty())
					{
						g_GetHighlitedTextInstance->hTerminal->sendCommandToCMD(command, true);
					}
				}
			}
		}
	}

	return CallNextHookEx(g_GetHighlitedTextInstance->hHook, nCode, wParam, lParam);
}