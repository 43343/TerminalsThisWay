#include "inputField.h"
#include <iostream>
#include <Uxtheme.h>

namespace GUI {
	InputField::InputField(HINSTANCE hInstance, int x, int y, int width, int height, HWND parentHwnd)
		: hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {
		hwnd = CreateWindowExW(
			WS_EX_COMPOSITED | WS_EX_TRANSPARENT,
			L"EDIT",
			L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | WS_CLIPCHILDREN,
			x, y, width, height,
			parentHwnd,
			nullptr,
			hInstance,
			nullptr
		);

		SendMessageW(hwnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
		SendMessageW(hwnd, EM_LIMITTEXT, 100, 0);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		originalWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
		SetWindowTheme(hwnd, L"Explorer", nullptr);
	}
	std::wstring InputField::getInput() const {
		
		int length = GetWindowTextLengthW(hwnd);
		if (length == 0) {
			return L""; 
		}

		
		wchar_t* buffer = new wchar_t[length + 1]; 

		
		GetWindowTextW(hwnd, buffer, length + 1);

		
		std::wstring text(buffer);

		
		delete[] buffer;

		return text;
	}
	void InputField::setCallback(std::function<void()> cb)
	{
		callback = cb;

	}

	void InputField::setInput(const std::wstring& text) {
		SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
		SetWindowTextW(hwnd, text.c_str());
		SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(hwnd, NULL, NULL,
			RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
	void InputField::setLimit(int limit) {
		SendMessageW(hwnd, EM_LIMITTEXT, limit, 0);
	}

	HWND InputField::getHwnd() const {
		return hwnd;
	}

	LRESULT CALLBACK InputField::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		InputField* inputField = reinterpret_cast<InputField*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (uMsg) {
		case WM_ERASEBKGND:
			return 1;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rc;
			GetClientRect(hwnd, &rc);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

			FillRect(hdcMem, &rc, (HBRUSH)(COLOR_WINDOW + 1));
			CallWindowProc(inputField->originalWndProc, hwnd, WM_PRINT, (WPARAM)hdcMem,
				PRF_CLIENT | PRF_NONCLIENT);

			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);
			EndPaint(hwnd, &ps);
			return 0;
		}

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			return CallWindowProc(inputField->originalWndProc, hwnd, uMsg, wParam, lParam);

		case WM_KEYUP:
		case WM_CHAR:
			if (inputField->callback) {
				inputField->callback();
			}
			break;
		}

		return CallWindowProc(inputField->originalWndProc, hwnd, uMsg, wParam, lParam);
	}
}