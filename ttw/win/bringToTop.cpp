#include "bringToTop.h"
#include "Config/configManager.h"
#include <chrono>
#include <iostream>

BringToTop* g_BringToTopInstance = nullptr;
auto firstPressTimeBringToTop = std::chrono::steady_clock::now();

BringToTop::BringToTop(Terminal* cmd) : hTerminal(*cmd)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_BringToTopInstance = this;
}

BringToTop::~BringToTop()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}

LRESULT CALLBACK BringToTop::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (Simulating::IsSimulating())
		{
			return CallNextHookEx(g_BringToTopInstance->hHook, nCode, wParam, lParam);
		}
		if (ConfigManager::getInstance().getConfig().keyBringToTop2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyBringToTop1)
			{
				g_BringToTopInstance->hTerminal.bringProcessWindowToTop();
			}
		}
		else
		{
			if (ConfigManager::getInstance().getConfig().keyBringToTop1 == ConfigManager::getInstance().getConfig().keyBringToTop2)
			{
				if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyBringToTop1)
				{
					auto currentTime = std::chrono::steady_clock::now();
					if (!g_BringToTopInstance->firstPressDetected) {
						g_BringToTopInstance->firstPressDetected = true;
						firstPressTimeBringToTop = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTimeBringToTop).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							g_BringToTopInstance->hTerminal.bringProcessWindowToTop();
						}
						else {
							firstPressTimeBringToTop = currentTime;
						}
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyBringToTop1) {
						g_BringToTopInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyBringToTop2) {
						g_BringToTopInstance->keyB_pressed = true;
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyBringToTop1) {
						g_BringToTopInstance->keyA_pressed = false;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keyBringToTop2) {
						g_BringToTopInstance->keyB_pressed = false;
					}
				}

				if (g_BringToTopInstance->keyA_pressed && g_BringToTopInstance->keyB_pressed) {
					g_BringToTopInstance->hTerminal.bringProcessWindowToTop();
				}
			}
		}

	}

	return CallNextHookEx(g_BringToTopInstance->hHook, nCode, wParam, lParam);
}