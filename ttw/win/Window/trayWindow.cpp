#include "trayWindow.h"
#include <strsafe.h>
#include "../Config/configManager.h"
#include "../Utility/utility.h"
#include "about.h"

TrayWindow::TrayWindow(HINSTANCE& hInstance, HANDLE& mutex, Terminal* cmd) : hWnd(nullptr), hTerminal(*cmd), hMutex(mutex) {
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

	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	ShowWindow(hWnd, SW_HIDE);
}

TrayWindow::~TrayWindow() {
	DestroyWindow(hWnd);
}

LRESULT CALLBACK TrayWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static NOTIFYICONDATA nid;
	TrayWindow* pThis = reinterpret_cast<TrayWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
	static Settings* settingsWindow = nullptr;
	static About* aboutWindow = nullptr;

	switch (uMsg) {
	case WM_CREATE:
		pThis = reinterpret_cast<TrayWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uVersion = NOTIFYICON_VERSION;
		//nid.uID = 1;
		nid.uFlags = NIF_ICON | NIF_MESSAGE;
		nid.uCallbackMessage = WM_TRAYICON;
		nid.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(NULL), "IDI_ICON1");
		StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), TEXT("TerminalsThisWay"));
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;

	case WM_TRAYICON:
		if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN) {
			POINT pt;
			GetCursorPos(&pt);

			HMENU hMenu = CreatePopupMenu();
			MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
			mii.fMask = MIIM_STRING | MIIM_ID | MIIM_BITMAP;
			mii.wID = ID_TRAY_RESTART_ADMIN;
			mii.dwTypeData = "Restart as an administrator";
			SHSTOCKICONINFO sii = { sizeof(sii) };
			if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_ICON | SHGSI_SMALLICON, &sii))) {
				ICONINFO iconInfo = { 0 };
				if (!GetIconInfo(sii.hIcon, &iconInfo)) {
					return DefWindowProc(hwnd, uMsg, wParam, lParam);
				}

				BITMAP bmpColor = { 0 };
				GetObject(iconInfo.hbmColor, sizeof(bmpColor), &bmpColor);

				HDC hDC = GetDC(nullptr);
				HDC hMemDC = CreateCompatibleDC(hDC);
				HDC hMaskDC = CreateCompatibleDC(hDC);

				BITMAPINFO bmi = { 0 };
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = bmpColor.bmWidth;
				bmi.bmiHeader.biHeight = -bmpColor.bmHeight;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;

				void* pBits = nullptr;
				HBITMAP hAlphaBmp = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
				HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hAlphaBmp);

				DrawIconEx(hMemDC, 0, 0, sii.hIcon, bmpColor.bmWidth, bmpColor.bmHeight, 0, nullptr, DI_NORMAL);
				if (isRunningAsAdministrator()) {
					for (int y = 0; y < bmpColor.bmHeight; y++) {
						RGBQUAD* pRow = (RGBQUAD*)((BYTE*)pBits + y * bmpColor.bmWidth * 4);
						for (int x = 0; x < bmpColor.bmWidth; x++) {
							if (pRow[x].rgbReserved > 0) { 
								BYTE gray = (BYTE)(pRow[x].rgbRed * 0.299 +
									pRow[x].rgbGreen * 0.587 +
									pRow[x].rgbBlue * 0.114);
								pRow[x].rgbRed = gray;
								pRow[x].rgbGreen = gray;
								pRow[x].rgbBlue = gray;
							}
						}
					}
				}

				SelectObject(hMemDC, hOldBmp);
				DeleteDC(hMemDC);
				DeleteDC(hMaskDC);
				ReleaseDC(nullptr, hDC);
				DeleteObject(iconInfo.hbmColor);
				DeleteObject(iconInfo.hbmMask);
				mii.hbmpItem = hAlphaBmp;
				DestroyIcon(sii.hIcon);
			}
			InsertMenuItem(hMenu, 0, TRUE, &mii);
			if (isRunningAsAdministrator())
				EnableMenuItem(hMenu, ID_TRAY_RESTART_ADMIN, MF_BYCOMMAND | MF_GRAYED);
			AppendMenu(hMenu, MF_STRING, ID_TRAY_UPDATE, "Update configuration");
			AppendMenu(hMenu, MF_STRING, ID_TRAY_SETTINGS, "Change configuration");
			AppendMenu(hMenu, MF_STRING, ID_TRAY_ABOUT, "About");
			AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

			SetForegroundWindow(hwnd);
			TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
			ZeroMemory(&nid, sizeof(nid));
			DestroyMenu(hMenu);
			SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TRAY_RESTART_ADMIN:
			if (runAsAdministrator())
			{
				CloseHandle(pThis->hMutex);
				pThis->hTerminal.sendCommandToCMD(L"exit", false);
				exit(0);
			}
			break;
		case ID_TRAY_UPDATE:
			ConfigManager::getInstance().generateConfigFile(getAppdataFolder() + L"\\config.conf");
			ConfigManager::getInstance().loadFromFile(getAppdataFolder() + L"\\config.conf");
			if (settingsWindow)
			{
				settingsWindow->setInput();
			}
			break;
		case ID_TRAY_SETTINGS:
			if (!settingsWindow || !IsWindow(settingsWindow->getHwnd())) {
				settingsWindow = new Settings(GetModuleHandle(NULL));
				if (!settingsWindow->createWindow()) {
					delete settingsWindow;
					settingsWindow = nullptr;
				}
			}
			else {
				if (!IsWindowVisible(settingsWindow->getHwnd())) {
					ShowWindow(settingsWindow->getHwnd(), SW_SHOW);
				}
				SetForegroundWindow(settingsWindow->getHwnd());
			}
			break;
		case ID_TRAY_ABOUT:
			if (!aboutWindow || !IsWindow(aboutWindow->getHwnd())) {
				aboutWindow = new About(GetModuleHandle(NULL));
				if (!aboutWindow->createWindow()) {
					delete aboutWindow;
					aboutWindow = nullptr;
				}
			}
			else {
				if (!IsWindowVisible(aboutWindow->getHwnd())) {
					ShowWindow(aboutWindow->getHwnd(), SW_SHOW);
				}
				SetForegroundWindow(aboutWindow->getHwnd());
			}
			break;
		case ID_TRAY_EXIT:
			pThis->hTerminal.sendCommandToCMD(L"exit", false);
			Shell_NotifyIcon(NIM_DELETE, &nid);
			exit(0);
			break;
		}
		break;

	case WM_DESTROY:
		pThis->hTerminal.sendCommandToCMD(L"exit", false);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		std::cout << "lol";
		exit(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}