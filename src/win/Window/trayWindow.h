#pragma once
#include <windows.h>
#include <shellapi.h>
#include <cstdio>
#include "settings.h"
#include "../terminal.h"
#define WM_TRAYICON (WM_USER + 1)
#define IDI_ICON1 101
#define ID_TRAY_RESTART_ADMIN 1001
#define ID_TRAY_UPDATE 1002
#define ID_TRAY_SETTINGS 1003
#define ID_TRAY_ABOUT 1004
#define ID_TRAY_EXIT 1005

class TrayWindow {
public:
    TrayWindow(HINSTANCE& hInstance, Terminal* cmd);
    ~TrayWindow();

private:
    HWND hWnd;
    NOTIFYICONDATA nid;
    Terminal& hTerminal;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};