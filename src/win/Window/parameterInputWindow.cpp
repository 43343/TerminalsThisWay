#include "parameterInputWindow.h"
#include "../Config/configManager.h"
#include <chrono>
#include <iostream>

ParameterInputWindow* g_ParameterInputWindowInstance = nullptr;
auto firstPressTimeParameterInput = std::chrono::steady_clock::now();

ParameterInputWindow::ParameterInputWindow(Terminal* cmd, HINSTANCE instance, GetText& getText) : hTerminal(*cmd), hGetText(getText)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_ParameterInputWindowInstance = this;
}

ParameterInputWindow::~ParameterInputWindow()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}

void ParameterInputWindow::CreateInputWindow()
{
	POINT p;
	GetCursorPos(&p);
	const wchar_t CLASS_NAME[] = L"InputWindowClass";

	WNDCLASSW wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;

	RegisterClassW(&wc);

	hwnd = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		CLASS_NAME,
		L"",
		WS_POPUP,
		p.x, p.y, 200, 20,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	isDialogOpen = true;
	input = new GUI::InputField(hInstance, 0, 0, 200, 20, hwnd);
	std::wstring& command = hGetText.getSelectedText();
	input->setInput(command);

	SetFocus(input->getHwnd());
	ShowWindow(hwnd, SW_SHOWNORMAL);


}

LRESULT CALLBACK ParameterInputWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// static HWND hEdit;

	switch (uMsg) {
	case WM_CREATE:
		
	   /* hEdit = CreateWindowEx(0, "EDIT", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			0, 0, 200, 20, hwnd, NULL, NULL, NULL);*/

			// SetFocus(hEdit);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ParameterInputWindow::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (g_ParameterInputWindowInstance->isDialogOpen)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == VK_RETURN)
			{
				std::wstring command = g_ParameterInputWindowInstance->input->getInput();
				if (!command.empty())
				{
					command += L"\n";
					g_ParameterInputWindowInstance->hTerminal.sendCommandToCMD(command);
				}
				DestroyWindow(g_ParameterInputWindowInstance->hwnd);
				g_ParameterInputWindowInstance->isDialogOpen = false;
				//SetFocus(NULL);
			}
		}
		if (ConfigManager::getInstance().getConfig().keySendCommandParameter2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1)
			{
				if (!g_ParameterInputWindowInstance->isDialogOpen)
				{
					g_ParameterInputWindowInstance->CreateInputWindow();
				}
			}
		}
		else
		{
			if (ConfigManager::getInstance().getConfig().keySendCommandParameter1 == ConfigManager::getInstance().getConfig().keySendCommandParameter2)
			{
				if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1)
				{
					auto currentTime = std::chrono::steady_clock::now();
					if (!g_ParameterInputWindowInstance->firstPressDetected) {
						g_ParameterInputWindowInstance->firstPressDetected = true;
						firstPressTimeParameterInput = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTimeParameterInput).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							if (!g_ParameterInputWindowInstance->isDialogOpen)
							{
								g_ParameterInputWindowInstance->CreateInputWindow();
							}
						}
						else {
							firstPressTimeParameterInput = currentTime; 
						}
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1) { 
						g_ParameterInputWindowInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter2) { 
						g_ParameterInputWindowInstance->keyB_pressed = true;
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1) {
						g_ParameterInputWindowInstance->keyA_pressed = false;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter2) {
						g_ParameterInputWindowInstance->keyB_pressed = false;
					}
				}

				if (g_ParameterInputWindowInstance->keyA_pressed && g_ParameterInputWindowInstance->keyB_pressed) {
					std::cout << "gParameter1 " << g_ParameterInputWindowInstance->keyA_pressed << " gParameter2 " << g_ParameterInputWindowInstance->keyB_pressed;
					if (!g_ParameterInputWindowInstance->isDialogOpen)
					{
						g_ParameterInputWindowInstance->CreateInputWindow();;
					}
				}
			}
		}

	}

	return CallNextHookEx(g_ParameterInputWindowInstance->hHook, nCode, wParam, lParam);
}