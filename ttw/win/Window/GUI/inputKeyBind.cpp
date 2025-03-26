#include "inputKeyBind.h"
#include <iostream>
#include "../../Utility/utility.h"

namespace GUI {
	InputKeyBind::InputKeyBind(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height) : parentHwnd(parentHwnd) {

		hwnd = CreateWindowExW(
			0, L"EDIT",
			L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
			x, y, width, height,
			parentHwnd, nullptr, hInstance, nullptr);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
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

		DWORD vkCode = static_cast<DWORD>(wParam);
		if (msg == WM_SETFOCUS)
		{
			Simulating::SetSimulating(true);
		}
		if (msg == WM_KILLFOCUS)
		{
			Simulating::SetSimulating(false);
			SetWindowTextW(hwnd, L"");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			SetWindowTextW(hwnd, keyBind->keySequence.c_str());
		}
		if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
		{
			if (btnCount == 0) keyBind->keySequence.clear();                       
			SetWindowTextW(hwnd, L"");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			if (btnCount == 1) keyBind->keySequence += L" + ";    
			wchar_t keyName[32];
			keyBind->GetEnglishKeyNameText(wParam, lParam, keyName, sizeof(keyName) / sizeof(wchar_t));
			std::wcout << "keyAher:" << keyName;
			keyBind->keySequence += keyName;
			std::wcout << L"YES\n";
			SetWindowTextW(hwnd, keyBind->keySequence.c_str());
			btnCount++;
			if (btnCount == 2) SetFocus(NULL);
			if (wParam == VK_MENU) {
				return 0;
			}
		}
		if (msg == WM_LBUTTONUP)
		{
			SetWindowTextW(hwnd, L"");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			std::cout << "LBUTTON";
			SetWindowTextW(hwnd, L"Press the key");
			btnCount = 0;
		}
		return CallWindowProc(keyBind->originalWndProc, hwnd, msg, wParam, lParam);
	}
}