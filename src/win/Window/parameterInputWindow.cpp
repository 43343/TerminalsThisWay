#include "parameterInputWindow.h"
#include "../Config/configManager.h"
#include "../uiAutomation.h"
#include <chrono>
#include <iostream>

ParameterInputWindow* g_ParameterInputWindowInstance = nullptr;
auto firstPressTimeParameterInput = std::chrono::steady_clock::now();

ParameterInputWindow::ParameterInputWindow(Terminal* cmd, HINSTANCE instance, UIAutomation* uiAutomation) : hTerminal(*cmd), hUiAutomation(*uiAutomation)
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
    const char CLASS_NAME[] = "InputWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        "",
        WS_POPUP,
        p.x, p.y, 200, 20,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    isDialogOpen = true;
    input = new GUI::InputField(hInstance, 0, 0, 200, 20, hwnd);
    SetFocus(hwnd);
    input->setInput(hUiAutomation.getSelectedText());
    ShowWindow(hwnd, SW_SHOWNORMAL);

}

LRESULT CALLBACK ParameterInputWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   // static HWND hEdit;

    switch (uMsg) {
    case WM_CREATE:
        // Создаем поле ввода
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
                std::string command = g_ParameterInputWindowInstance->input->getInput();
                if (!command.empty())
                {
                    command += "\r\n";
                    g_ParameterInputWindowInstance->hTerminal.sendCommandToCMD(command);
                }
                DestroyWindow(g_ParameterInputWindowInstance->hwnd);
                g_ParameterInputWindowInstance->isDialogOpen = false;
            }
        }
        else {
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
                                    std::cout << "I'm here";
                                    g_ParameterInputWindowInstance->CreateInputWindow();
                                }
                            }
                            else {
                                firstPressTimeParameterInput = currentTime; // Обновляем время первого нажатияvv
                            }
                        }
                    }
                }
                else
                {
                    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                        if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1) { // Код для 'A'
                            g_ParameterInputWindowInstance->keyA_pressed = true;
                        }
                        if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter2) { // Код для 'B'
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
                        if (!g_ParameterInputWindowInstance->isDialogOpen)
                        {
                            g_ParameterInputWindowInstance->CreateInputWindow();;
                        }
                    }
                }
            }
        }
    }

    return CallNextHookEx(g_ParameterInputWindowInstance->hHook, nCode, wParam, lParam);
}