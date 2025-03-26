#include "image.h"
#include <iostream>

namespace GUI {
	Image::Image(HINSTANCE hinstance, HWND parentHwnd, int x, int y, int width, int height) : hInstance(hinstance)
	{
		hwnd = CreateWindowEx(
			0,
			"Static",
			nullptr,
			SS_ICON | WS_CHILD | WS_VISIBLE,  
			x, y, width, height,
			parentHwnd,
			nullptr,
			hinstance,
			nullptr
		);
	} 
	Image::~Image() {
		if (hCurrentIcon) {
			DestroyIcon(hCurrentIcon);
		}
	}
	void Image::setIcon(LPCSTR resourceId)
	{
		if (hCurrentIcon) {
			DestroyIcon(hCurrentIcon);
			hCurrentIcon = nullptr;
		}

		HICON hIcon = LoadIcon(hInstance, resourceId);
		if (hIcon) {
			hCurrentIcon = hIcon;
			SendMessage(hwnd, STM_SETICON, (WPARAM)hIcon, 0);
		}
	}
}