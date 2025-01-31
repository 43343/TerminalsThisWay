#include "inputField.h"
#include <iostream>

namespace GUI {
	InputField::InputField(HINSTANCE hInstance, int x, int y, int width, int height, HWND parentHwnd)
		: hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {
		hwnd = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
			x, y, width, height,
			parentHwnd,
			nullptr,
			hInstance,
			nullptr
		);

		SendMessageW(hwnd, EM_LIMITTEXT, 100, 0);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		originalWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
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
		SetWindowTextW(hwnd, text.c_str());
	}
	void InputField::SetLimit(int limit) {
		SendMessageW(hwnd, EM_LIMITTEXT, limit, 0);
	}

	HWND InputField::getHwnd() const {
		return hwnd;
	}

	LRESULT CALLBACK InputField::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		InputField* inputField = reinterpret_cast<InputField*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (uMsg == WM_KEYUP) {
			if (inputField->callback)
				inputField->callback();
		}
		return CallWindowProc(inputField->originalWndProc, hwnd, uMsg, wParam, lParam);
	}
}