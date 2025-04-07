#include "inputKeyBind.h"
#include <iostream>
#include <Uxtheme.h>
#include "../../Utility/utility.h"

namespace GUI {
	InputKeyBind::InputKeyBind(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height) : parentHwnd(parentHwnd) {

		hwnd = CreateWindowExW(
			WS_EX_COMPOSITED | WS_EX_TRANSPARENT, L"EDIT",
			L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY | WS_CLIPCHILDREN,
			x, y, width, height,
			parentHwnd, nullptr, hInstance, nullptr);
		SendMessageW(hwnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
		SetWindowTheme(hwnd, L"Explorer", nullptr);
	}

	void InputKeyBind::HandleParentClick(POINT pt) {
		RECT rect;
		GetWindowRect(hwnd, &rect);

		POINT ptLeft = { rect.left, rect.top };
		POINT ptRight = { rect.right, rect.bottom };

		ScreenToClient(GetParent(hwnd), &ptLeft);
		ScreenToClient(GetParent(hwnd), &ptRight);

		RECT clientRect = { ptLeft.x, ptLeft.y, ptRight.x, ptRight.y };

		if (!PtInRect(&clientRect, pt)) {
			SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
			SetWindowTextW(hwnd, L"");
			SetWindowTextW(hwnd, keySequence.c_str());
			SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);

			InvalidateRect(GetParent(hwnd), &clientRect, FALSE);
			UpdateWindow(GetParent(hwnd));
		}
	}

	void InputKeyBind::setText(std::wstring text) {
		keySequence = text;
		SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
		SetWindowTextW(hwnd, keySequence.c_str());
		SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
		RECT rc;
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, FALSE);
	}
	std::wstring InputKeyBind::getText() {
		return keySequence;
	}
	void InputKeyBind::GetEnglishKeyNameText(WPARAM wParam, LPARAM lParam, wchar_t* keyName, int keyNameSize)
	{
		DWORD vkCode = static_cast<DWORD>(wParam);

		
		if (vkCode == VK_SHIFT) {
			bool isLeftShift = GetAsyncKeyState(VK_LSHIFT) & 0x8000;
			bool isRightShift = GetAsyncKeyState(VK_RSHIFT) & 0x8000;

			if (isLeftShift && !isRightShift) {
				vkCode = VK_LSHIFT;
			}
			else if (!isLeftShift && isRightShift) {
				vkCode = VK_RSHIFT;
			}
		}

		
		if (vkCode == VK_CONTROL) {
			bool isLeftCtrl = GetAsyncKeyState(VK_LCONTROL) & 0x8000;
			bool isRightCtrl = GetAsyncKeyState(VK_RCONTROL) & 0x8000;

			if (isLeftCtrl && !isRightCtrl) {
				vkCode = VK_LCONTROL;
			}
			else if (!isLeftCtrl && isRightCtrl) {
				vkCode = VK_RCONTROL;
			}
		}

		
		if (vkCode == VK_MENU) {
			bool isLeftMenu = GetAsyncKeyState(VK_LMENU) & 0x8000;
			bool isRightMenu = GetAsyncKeyState(VK_RMENU) & 0x8000;

			if (isLeftMenu && !isRightMenu) {
				vkCode = VK_LMENU;
			}
			else if (!isLeftMenu && isRightMenu) {
				vkCode = VK_RMENU;
			}
		}

		
		std::wstring hKey = keyToString(vkCode); 

		wprintf(L"Key Pressed: 0x%02X\n", vkCode); 

		if (!hKey.empty()) {
			wcsncpy_s(keyName, keyNameSize, hKey.c_str(), _TRUNCATE);
		}
		else {
			
			_snwprintf_s(keyName, keyNameSize, _TRUNCATE, L"Unknown (0x%02X)", vkCode);
		}
	}

	LRESULT CALLBACK InputKeyBind::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		InputKeyBind* keyBind = reinterpret_cast<InputKeyBind*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		static int btnCount = 0;
		static bool simulating = false;

		DWORD vkCode = static_cast<DWORD>(wParam);
		if (msg == WM_PAINT)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rc;
			GetClientRect(hwnd, &rc);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

			FillRect(hdcMem, &rc, (HBRUSH)(COLOR_WINDOW + 1));
			CallWindowProc(keyBind->originalWndProc, hwnd, WM_PRINT, (WPARAM)hdcMem,
				PRF_CLIENT | PRF_NONCLIENT);

			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);
			EndPaint(hwnd, &ps);
			return 0;
		}
		if (msg == WM_SETFOCUS)
		{
			Simulating::SetSimulating(true);
			simulating = true;
		}
		if (msg == WM_KILLFOCUS)
		{
			simulating = false;
			Simulating::SetSimulating(false);
			SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
			SetWindowTextW(hwnd, keyBind->keySequence.c_str());
			SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
		{
			if (btnCount < 2)
			{
				if (btnCount == 0) keyBind->keySequence.clear();
				SetWindowTextW(hwnd, L"");
				if (btnCount == 1) keyBind->keySequence += L" + ";
				wchar_t keyName[32];
				keyBind->GetEnglishKeyNameText(wParam, lParam, keyName, sizeof(keyName) / sizeof(wchar_t));
				keyBind->keySequence += keyName;
				SetWindowTextW(hwnd, keyBind->keySequence.c_str());
				btnCount++;
				if (btnCount == 2) simulating = false;
				SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
				InvalidateRect(hwnd, NULL, FALSE);
				if (wParam == VK_MENU) {
					return 0;
				}
			}
		}
		if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
		{
			if (!simulating)
			{
				SetFocus(NULL);
				Simulating::SetSimulating(false);
			}
		}
		if (msg == WM_LBUTTONUP)
		{
			SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
			SetWindowTextW(hwnd, L"Press the key");
			SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(hwnd, NULL, FALSE);
			btnCount = 0;
		}
		return CallWindowProc(keyBind->originalWndProc, hwnd, msg, wParam, lParam);
	}
}