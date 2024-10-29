#include "toggle.h"
#include <iostream>

namespace GUI {
	Toggle::Toggle(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height)
		: hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {
		hwnd = CreateWindowEx(0,
			"BUTTON",
			"",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			x, y, width, height,
			parentHwnd,
			NULL,
			hInstance,
			nullptr);

		setState(true);
	}

	bool Toggle::getState() const {
		return SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}

	void Toggle::setState(bool state) {
		SendMessage(hwnd, BM_SETCHECK, state ? BST_CHECKED : BST_UNCHECKED, 0);
	}


}