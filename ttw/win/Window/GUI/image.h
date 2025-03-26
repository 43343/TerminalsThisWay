#pragma once
#include <windows.h>
#include <string>

namespace GUI
{
	class Image
	{
	public:
		Image(HINSTANCE hinstance, HWND parentHwnd, int x, int y, int width, int height);
		void setIcon(LPCSTR);
		~Image();
	private:
		HWND hwnd;
		HINSTANCE hInstance;
		HICON hCurrentIcon = nullptr;
	};
}
