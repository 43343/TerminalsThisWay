#include "trayWindow.h"
#include <cstring>
#include <shellapi.h>
#include "../Config/configManager.h"
#include "../Utility/utility.h"
#include "about.h"

TrayWindow* g_TrayWindow = nullptr;

TrayWindow::TrayWindow(HINSTANCE& hInstance, Terminal* cmd) : hWnd(nullptr), hTerminal(*cmd) {
	const char CLASS_NAME[] = "TrayAppClass";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"Tray Application", 0,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		NULL,
		hInstance,
		this
	);

	ShowWindow(hWnd, SW_HIDE);

	g_TrayWindow = this;

	//AddTrayIcon();
}

TrayWindow::~TrayWindow() {
	DestroyWindow(hWnd);
}
LRESULT CALLBACK TrayWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static NOTIFYICONDATA nid;
	static Settings* settingsWindow = nullptr;
	static About* aboutWindow = nullptr;

	switch (uMsg) {
	case WM_CREATE:
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uVersion = NOTIFYICON_VERSION;
		//nid.uID = 1;
		nid.uFlags = NIF_ICON | NIF_MESSAGE;
		nid.uCallbackMessage = WM_TRAYICON;
		nid.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(NULL), "IDI_ICON1");
		lstrcpy(nid.szTip, TEXT("TerminalsThisWay"));
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;

	case WM_TRAYICON:
		if (lParam == WM_RBUTTONDOWN) {
			POINT pt;
			GetCursorPos(&pt);

			HMENU hMenu = CreatePopupMenu();
			HICON hIcon = NULL;
			SHSTOCKICONINFO sii = { sizeof(sii) };
			if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_ICON | SHGSI_SMALLICON, &sii))) {
				hIcon = sii.hIcon;
			}

			/*LPSTR s = const_cast<char*>(local->getText("restartAsAnAdministrator").c_str());
			MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
			mii.fMask = MIIM_STRING | MIIM_ID | MIIM_BITMAP;
			mii.wID = ID_TRAY_RESTART_ADMIN;
			mii.dwTypeData = s;
			HBITMAP hBitmap = nullptr;
			ICONINFO iconInfo;
			if (GetIconInfo(hIcon, &iconInfo))
			{
				hBitmap = iconInfo.hbmColor; // We use the color part of the icon
				//DeleteObject(iconInfo.hbmMask); // We remove the mask if it is not needed
			}
			if (hBitmap)
			{
				mii.hbmpItem = hBitmap; // Installing HBITMAP in the menu item

				// Add a menu item to the menu
				InsertMenuItem(hMenu, 0, TRUE, &mii);

				// Release the HBITMAP if it is no longer needed
				//DeleteObject(hBitmap);
			}

			// Release the icon after use
			//DestroyIcon(hIcon);*/
			AppendMenu(hMenu, MF_STRING, ID_TRAY_RESTART_ADMIN, "Restart as an administrator");
			if (isRunningAsAdministrator())
				EnableMenuItem(hMenu, ID_TRAY_RESTART_ADMIN, MF_BYCOMMAND | MF_GRAYED);
			AppendMenu(hMenu, MF_STRING, ID_TRAY_UPDATE, "Update configuration");
			AppendMenu(hMenu, MF_STRING, ID_TRAY_SETTINGS, "Change configuration");
			AppendMenu(hMenu, MF_STRING, ID_TRAY_ABOUT, "About");
			AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

			SetForegroundWindow(hwnd);
			TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
			DestroyMenu(hMenu);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TRAY_RESTART_ADMIN:
			if (runAsAdministrator())
			{
				exit(0);
				g_TrayWindow->hTerminal.sendCommandToCMD("exit\r\n");
			}
			break;
		case ID_TRAY_UPDATE:
			ConfigManager::getInstance().generateConfigFile("config.conf");
			ConfigManager::getInstance().loadFromFile("config.conf");
			if (settingsWindow)
			{
				settingsWindow->setInput();
			}
			break;
		case ID_TRAY_SETTINGS:
			if (!settingsWindow) {
				settingsWindow = new Settings(GetModuleHandle(NULL));
				if (!settingsWindow->createWindow()) {
					delete settingsWindow;
					settingsWindow = nullptr;
				}
			}
			else {
				if (!IsWindowVisible(settingsWindow->hwnd)) {
					ShowWindow(settingsWindow->hwnd, SW_SHOW);
				}
				SetForegroundWindow(settingsWindow->hwnd);
			}
			break;
		case ID_TRAY_ABOUT:
			if (!aboutWindow) {
				aboutWindow = new About(GetModuleHandle(NULL));
				if (!aboutWindow->createWindow()) {
					delete aboutWindow;
					aboutWindow = nullptr;
				}
			}
			else {
				if (!IsWindowVisible(aboutWindow->hwnd)) {
					ShowWindow(aboutWindow->hwnd, SW_SHOW);
				}
				SetForegroundWindow(aboutWindow->hwnd);
			}
			break;
		case ID_TRAY_EXIT:
			g_TrayWindow->hTerminal.sendCommandToCMD("exit\r\n");
			Shell_NotifyIcon(NIM_DELETE, &nid);
			exit(0);
			break;
		}
		break;

	case WM_DESTROY:
		g_TrayWindow->hTerminal.sendCommandToCMD("exit\r\n");
		Shell_NotifyIcon(NIM_DELETE, &nid);
		exit(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}