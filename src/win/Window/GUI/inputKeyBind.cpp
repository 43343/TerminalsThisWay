#include "inputKeyBind.h"
#include <iostream>
#include <tchar.h>
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
		ScreenToClient(GetParent(hwnd), (LPPOINT)&rect.left);
		ScreenToClient(GetParent(hwnd), (LPPOINT)&rect.right);

		if (!PtInRect(&rect, pt)) {
			std::cout << keySequence.c_str();
			SetWindowText(hwnd, "");
			InvalidateRect(parentHwnd, NULL, TRUE);
			UpdateWindow(parentHwnd);
			SetWindowText(hwnd, keySequence.c_str());
		}
	}

	void InputKeyBind::setText(std::string text) {
		keySequence = text;
		SetWindowText(hwnd, keySequence.c_str());
	}
	std::string InputKeyBind::getText() {
		return keySequence;
	}

	void InputKeyBind::RemoveSpaces(char* str) {
		char* p1 = str; // Pointer to iterate over the original string
		char* p2 = str; // Pointer to build the new string without spaces

		while (*p1 != L'\0') {
			if (*p1 != L' ') {
				*p2++ = *p1; // Copy non-space character
			}
			p1++;
		}
		*p2 = L'\0'; // Null-terminate the modified string
	}
	void InputKeyBind::GetEnglishKeyNameText(WPARAM wParam, LPARAM lParam, char* keyName, int keyNameSize)
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
		std::string hKey = keyToString(vkCode);
		printf("Key Pressed: 0x%02Xn", vkCode);
		if (hKey != "") {
			strncpy_s(keyName, keyNameSize, hKey.c_str(), _TRUNCATE);
		}
		else {
			// Если клавиша не найдена в таблице сопоставления
			_snprintf_s(keyName, keyNameSize, _TRUNCATE, "Unknown (0x%02X)", vkCode);
		}
	}
	LRESULT CALLBACK InputKeyBind::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		InputKeyBind* keyBind = reinterpret_cast<InputKeyBind*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		static int btnCount = 0;

		DWORD vkCode = static_cast<DWORD>(wParam);
		if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
		{
			if (btnCount == 0) keyBind->keySequence.clear();
			SetWindowText(hwnd, "");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			if (btnCount == 1) keyBind->keySequence += " + ";
			char keyName[32];
			keyBind->GetEnglishKeyNameText(wParam, lParam, keyName, sizeof(keyName) / sizeof(wchar_t));
			keyBind->RemoveSpaces(keyName);
			keyBind->keySequence += keyName;
			std::cout << keyBind;
			std::wcout << L"YES\n";
			SetWindowText(hwnd, keyBind->keySequence.c_str());
			btnCount++;
			if (btnCount == 2) SetFocus(NULL);
			if (wParam == VK_MENU)
			{
				return 0;
			}
		}
		if (msg == WM_LBUTTONUP)
		{
			SetWindowText(hwnd, "");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			//keyBind->keySequence.clear();
			std::cout << "LBUTTON";
			SetWindowText(hwnd, "Нажмите клавишу");
			btnCount = 0;
		}
		return CallWindowProc(keyBind->originalWndProc, hwnd, msg, wParam, lParam);
	}
}