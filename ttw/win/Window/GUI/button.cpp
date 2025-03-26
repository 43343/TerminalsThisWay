#include "button.h"
#include <iostream>
#include <shellapi.h>
#include <commctrl.h>

namespace GUI {
	Button::Button(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height)
		: hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {

		hwnd = CreateWindowEx(
			0,
			"BUTTON",
			"",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON ,
			x, y, width, height,
			parentHwnd,
			nullptr,
			(HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE),
			nullptr
		);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		originalProc = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ButtonProc)));
	}

	HWND Button::getHwnd() const {
		return hwnd;
	}
	void Button::setCallback(std::function<void()> cb)
	{
		callback = cb;

	}
	void Button::setText(const std::string& text) {
		SetWindowText(hwnd, text.c_str());
	}
	void Button::setSystemIcon(SHSTOCKICONID iconId) {
		HICON hIcon;

		SHSTOCKICONINFO sii = { sizeof(sii) };
		if (SUCCEEDED(SHGetStockIconInfo(iconId, SHGSI_ICON | SHGSI_SMALLICON, &sii))) {
			hIcon = sii.hIcon;

			// Добавляем стиль BS_ICON
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
			SetWindowLongPtr(hwnd, GWL_STYLE, style | BS_ICON);
			SendMessage(hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

			// Центрируем иконку (альтернативный способ без BM_SETMARGINS)
			RECT rc;
			GetClientRect(hwnd, &rc);
			rc.left += 5;
			InflateRect(&rc, -5, 0);
			SendMessage(hwnd, BCM_SETTEXTMARGIN, 0, (LPARAM)&rc);

			InvalidateRect(hwnd, nullptr, TRUE);
		}
	}
	void Button::setFontSize(int fontSize) {
		if (hFont) {
			DeleteObject(hFont);
		}

		int height = -MulDiv(fontSize, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);

		hFont = CreateFont(
			height, 0, 0, 0,
			FW_BOLD, 
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			VARIABLE_PITCH,
			TEXT("Arial")
		);

		SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
		InvalidateRect(hwnd, nullptr, TRUE); 
	}
	LRESULT CALLBACK Button::ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		Button* btn = reinterpret_cast<Button*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (btn) {
			if (msg == WM_LBUTTONUP) {
				std::cout << "test";
				if (btn && btn->callback) {
					btn->callback();
				}
			}
			return CallWindowProc(btn->originalProc, hwnd, msg, wParam, lParam);
		}
		else return CallWindowProc(nullptr, nullptr, msg, wParam, lParam);
	}
}