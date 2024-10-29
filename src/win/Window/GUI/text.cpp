#include "text.h"
#include <iostream>

namespace GUI {
	Text::Text(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height)
		: hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {
		hwnd = CreateWindowEx(
			0,
			"STATIC",
			"",
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			x, y, width, height,
			parentHwnd,
			nullptr,
			hInstance,
			nullptr
		);
	}

	HWND Text::getHwnd() const {
		return hwnd;
	}
	void Text::setAllign(DWORD alignStyle)
	{
		LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
		style &= ~(SS_LEFT | SS_CENTER | SS_RIGHT);
		style |= alignStyle;
		SetWindowLongPtr(hwnd, GWL_STYLE, style);
		InvalidateRect(hwnd, nullptr, TRUE);

	}
	void Text::setVisible(bool visible) {
		ShowWindow(hwnd, visible ? SW_SHOW : SW_HIDE);
	}

	void Text::setText(const std::string& text) {
		SetWindowText(hwnd, text.c_str());
		InvalidateRect(hwnd, nullptr, TRUE);
	}
	void Text::setColor(const std::string& hex) {
		// Convert hex string to COLORREF
		std::cout << hex.size();
		if (hex.size() != 6) return; // Invalid hex color
		int r = std::stoi(hex.substr(0, 2), nullptr, 16);
		int g = std::stoi(hex.substr(2, 2), nullptr, 16);
		int b = std::stoi(hex.substr(4, 2), nullptr, 16);
		std::cout << r << " " << g << " " << b;
		COLORREF color = RGB(r, g, b);

		// Set the text color
		HDC hdc = GetDC(hwnd);
		SetTextColor(hdc, color);
		ReleaseDC(hwnd, hdc);

		// Redraw the window
		InvalidateRect(parentHwnd, nullptr, TRUE);
	}

	void Text::setStyle(bool bold, bool italic) {
		this->bold = bold;
		this->bold = bold;
		setFont();
	}
	void Text::setFontSize(int fontSize) {
		this->fontSize = fontSize;
		setFont();
	}
	void Text::setFont() {
		if (hFont) {
			DeleteObject(hFont);
		}

		int height = -MulDiv(fontSize, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
		int weight = bold ? FW_BOLD : FW_NORMAL;

		hFont = CreateFont(
			height, 0, 0, 0,
			weight, // или используйте текущие параметры bold/italic
			italic ? TRUE : FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			VARIABLE_PITCH,
			TEXT("Arial")
		);

		SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
		InvalidateRect(hwnd, nullptr, TRUE); // Обновление окна
	}
}