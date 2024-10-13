#include "inputKeyBind.h"
#include <iostream>
#include <tchar.h>

namespace GUI {
	InputKeyBind::InputKeyBind(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height) : parentHwnd(parentHwnd) {

		HWND hwnd = CreateWindowExW(
			0, L"EDIT",   
			L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
			x, y, width, height,
			parentHwnd, NULL, hInstance, NULL);
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
	void InputKeyBind::GetEnglishKeyNameText(WPARAM wParam, char* keyName, int keyNameSize)
	{
		UINT virtualKey = static_cast<UINT>(wParam);
		HKL englishLayout = LoadKeyboardLayout(_T("00000409"), KLF_ACTIVATE);
		UINT scanCode = MapVirtualKeyEx(virtualKey, MAPVK_VK_TO_VSC, englishLayout);

		// Обработка специальных клавиш
		if (virtualKey == VK_NUMLOCK) {
			_tcscpy_s(keyName, keyNameSize, _T("NumLock"));
		}
		else if (virtualKey == VK_PAUSE) {
			_tcscpy_s(keyName, keyNameSize, _T("Pause"));
		}
		else if (virtualKey == VK_INSERT) {
			_tcscpy_s(keyName, keyNameSize, _T("Insert"));
		}
		else if (virtualKey == VK_HOME) {
			_tcscpy_s(keyName, keyNameSize, _T("Home"));
		}
		else if (virtualKey == VK_PRIOR) {
			_tcscpy_s(keyName, keyNameSize, _T("PageUp"));
		}
		else if (virtualKey == VK_DELETE) {
			_tcscpy_s(keyName, keyNameSize, _T("Delete"));
		}
		else if (virtualKey == VK_END) {
			_tcscpy_s(keyName, keyNameSize, _T("End"));
		}
		else if (virtualKey == VK_NEXT) {
			_tcscpy_s(keyName, keyNameSize, _T("PageDown"));
		}
		else {
			// Общая обработка для остальных клавиш
			LPARAM englishLParam = (scanCode << 16);
			if (GetKeyNameText(englishLParam, keyName, keyNameSize) == 0) {
				_tcscpy_s(keyName, keyNameSize, _T("Unknown"));
			}
		}
		UnloadKeyboardLayout(englishLayout);
	}
	LRESULT CALLBACK InputKeyBind::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		InputKeyBind* keyBind = reinterpret_cast<InputKeyBind*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		static int btnCount = 0;
		switch (msg) {
		case WM_KEYDOWN:
			
			if (btnCount == 0) keyBind->keySequence.clear();
			SetWindowText(hwnd, "");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			if (btnCount == 1) keyBind->keySequence += " + ";
			char keyName[32];
			keyBind->GetEnglishKeyNameText(wParam, keyName, sizeof(keyName) / sizeof(wchar_t));
			keyBind->RemoveSpaces(keyName);
			keyBind->keySequence += keyName;
			std::cout << keyBind;
			std::wcout << L"YES\n";
			SetWindowText(hwnd, keyBind->keySequence.c_str());
			btnCount++;
			if (btnCount == 2) SetFocus(NULL);
			break;
		case WM_LBUTTONUP:
			SetWindowText(hwnd, "");
			InvalidateRect(keyBind->parentHwnd, NULL, TRUE);
			UpdateWindow(keyBind->parentHwnd);
			//keyBind->keySequence.clear();
			std::cout << "LBUTTON";
			SetWindowText(hwnd, "Нажмите клавишу");
			btnCount = 0;
			//SetFocus(NULL);
			break;
		}
		return CallWindowProc(keyBind->originalWndProc, hwnd, msg, wParam, lParam);
	}
}