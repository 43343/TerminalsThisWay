#include "uiAutomation.h"
#include <iostream>
#include "Config//configManager.h"

UIAutomation* g_UIAutomationInstance = nullptr;
auto firstPressTime = std::chrono::steady_clock::now();

UIAutomation::UIAutomation(Terminal* cmd) : hTerminal(cmd)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_UIAutomationInstance = this;
}
UIAutomation::UIAutomation() : hTerminal(nullptr)
{
}
UIAutomation::~UIAutomation()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}
std::string UIAutomation::getSelectedText()
{
	std::string text = "";
	CoInitialize(NULL);

	IUIAutomation* pAutomation = NULL;
	IUIAutomationElement* pElement = NULL;
	IUIAutomationTextPattern* pTextPattern = NULL;
	IUIAutomationTextRangeArray* pTextRanges = NULL;

	// Create the UI Automation object
	if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pAutomation))) {
		// Get the focused element
		if (SUCCEEDED(pAutomation->GetFocusedElement(&pElement))) {
			VARIANT isPatternAvailable;
			if (SUCCEEDED(pElement->GetCurrentPropertyValue(UIA_IsTextPatternAvailablePropertyId, &isPatternAvailable)) && isPatternAvailable.boolVal == VARIANT_TRUE) {
				// Get the text pattern
				if (SUCCEEDED(pElement->GetCurrentPatternAs(UIA_TextPatternId, IID_IUIAutomationTextPattern, (void**)&pTextPattern))) {
					// Get the selection
					if (SUCCEEDED(pTextPattern->GetSelection(&pTextRanges))) {
						int count;
						if (SUCCEEDED(pTextRanges->get_Length(&count))) {
							for (int i = 0; i < count; i++) {
								IUIAutomationTextRange* pTextRange = NULL;
								if (SUCCEEDED(pTextRanges->GetElement(i, &pTextRange))) {
									BSTR bstrText;
									if (SUCCEEDED(pTextRange->GetText(-1, &bstrText))) {
										std::wcout << L"Selected Text: " << bstrText << std::endl;
										std::wstring wstr(bstrText, SysStringLen(bstrText));
										std::string text1(wstr.begin(), wstr.end());
										text = text1;
										SysFreeString(bstrText);
									}
									pTextRange->Release();
								}
							}
						}
						pTextRanges->Release();
					}
					pTextPattern->Release();
				}
				else {
					std::cerr << "Failed to get text pattern." << std::endl;
				}
			}
			else {
				std::cerr << "Text pattern is not available on the focused element." << std::endl;
			}
			pElement->Release();
		}
		else {
			std::cerr << "Failed to get focused element." << std::endl;
		}
		pAutomation->Release();
	}
	else {
		std::cerr << "Failed to create UI Automation instance." << std::endl;
	}

	// Uninitialize COM library
	CoUninitialize();
	return text;
}
LRESULT CALLBACK UIAutomation::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		std::cout << "key " << ConfigManager::getInstance().getConfig().keySendCommand2;
		if (ConfigManager::getInstance().getConfig().keySendCommand2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1)
			{
				std::string command = g_UIAutomationInstance->getSelectedText();
				if (!command.empty())
				{
					command += "\r\n";
					g_UIAutomationInstance->hTerminal->sendCommandToCMD(command);
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
					if (!g_UIAutomationInstance->firstPressDetected) {
						g_UIAutomationInstance->firstPressDetected = true;
						firstPressTime = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTime).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							std::string command = g_UIAutomationInstance->getSelectedText();
							if (!command.empty())
							{
								command += "\r\n";
								g_UIAutomationInstance->hTerminal->sendCommandToCMD(command);
							}
							g_UIAutomationInstance->firstPressDetected = false;
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
						g_UIAutomationInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand2) { 
						g_UIAutomationInstance->keyB_pressed = true;
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand1) {
						g_UIAutomationInstance->keyA_pressed = false;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommand2) {
						g_UIAutomationInstance->keyB_pressed = false;
					}
				}

				if (g_UIAutomationInstance->keyA_pressed && g_UIAutomationInstance->keyB_pressed) {
					std::cout << "Êîìáèíàöèÿ A + B íàæàòà!" << std::endl;
					std::string command = g_UIAutomationInstance->getSelectedText();
					if (!command.empty())
					{
						command += "\r\n";
						g_UIAutomationInstance->hTerminal->sendCommandToCMD(command);
					}
				}
			}
		}
	}

	return CallNextHookEx(g_UIAutomationInstance->hHook, nCode, wParam, lParam);
}