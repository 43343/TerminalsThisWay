#include "image.h"
#include <iostream>

namespace GUI {
	Image::Image(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height, std::string resourceId)
	{
		hwnd = CreateWindowEx(
			0,                      // Optional window styles.
			"Static",              // Predefined class; Unicode assumed.
			nullptr,                // No text.
			SS_ICON | WS_CHILD | WS_VISIBLE,  // Styles.
			x, y, width, height,         // Position and size.
			parentHwnd,                   // Parent window.
			nullptr,                // No menu.
			hInstance,              // Instance handle.
			nullptr                 // Additional application data.
		);
		HICON hIcon = LoadIcon(hInstance, "IDI_ICON1");
		SendMessage(hwnd, STM_SETICON, (WPARAM)hIcon, 0);
	}
	Image::~Image() {
	}

}